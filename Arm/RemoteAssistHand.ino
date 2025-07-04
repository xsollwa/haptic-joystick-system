#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <ESPmDNS.h>  // Added for mDNS
#include "HX711.h"
#include "main_page.h"

// Wi-Fi
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Servers
WebServer        httpServer(80);
WebSocketsServer webSocket(81);

// HX711 for arm's load cell
#define ARM_HX711_DT 18
#define ARM_HX711_SCK 19
HX711       armScale;

// Pins for motors
#define STEP_PIN   14
#define DIR_PIN    27
#define RST_PIN    12

#define SHOULDER_PIN   32
#define ELBOW_PIN      33
#define WRIST_PIN      25
#define GRASPER_PIN    26

// Motion params
const float STEPPER_MAX_SPEED    = 800.0;
const float STEPPER_ACCELERATION = 400.0;
const float SERVO_STEP_DEG       =   2.0;
const float SERVO_MIN_ANGLE      =   0.0;
const float SERVO_MAX_ANGLE      = 270.0;
const float ARM_FORCE_CAL        =   1.0;    // Calibrate this value
const float FORCE_INTERVAL_MS    = 200.0;
const float JOY_DEADZONE         =   0.1;    // Added deadzone for joystick

// Objects & state
AccelStepper baseStepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
Servo       shoulder, elbow, wrist, grasper;
unsigned long lastForceTime = 0;

// Joystick & controller state
float joy1x=0, joy1y=0, joy2x=0, joy2y=0;
float remoteForce = 0;
bool  discreteMode = true;

// Current angles for servos
float shoulderAngle = SERVO_MAX_ANGLE/2,
      elbowAngle    = SERVO_MAX_ANGLE/2,
      wristAngle    = SERVO_MAX_ANGLE/2,
      grasperAngle  = SERVO_MAX_ANGLE/2;

void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void broadcastForce();

void setup(){
  Serial.begin(115200);
  // Motor driver reset
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH);
  
  // Configure stepper
  baseStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  baseStepper.setAcceleration(STEPPER_ACCELERATION);
  
  // Attach servos
  shoulder.attach(SHOULDER_PIN, 500, 2500);
  elbow   .attach(ELBOW_PIN,    500, 2500);
  wrist   .attach(WRIST_PIN,    500, 2500);
  grasper .attach(GRASPER_PIN,  500, 2500);
  
  // Center servos
  for (Servo* s : { &shoulder, &elbow, &wrist, &grasper }) {
    int pulse = map((int)(SERVO_MAX_ANGLE/2),0,(int)SERVO_MAX_ANGLE,500,2500);
    s->writeMicroseconds(pulse);
  }
  
  // HX711
  armScale.begin(ARM_HX711_DT, ARM_HX711_SCK);
  armScale.set_scale(1.0);
  armScale.tare();
  
  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting Wi-Fi");
  while (WiFi.status()!=WL_CONNECTED) {
    delay(300);
    Serial.print('.');
  }
  Serial.println(" OK");
  
  // mDNS for hostname resolution
  if (!MDNS.begin("arm")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
  
  // HTTP server
  httpServer.on("/", HTTP_GET, [](){
    httpServer.send_P(200, "text/html", MAIN_PAGE);
  });
  httpServer.begin();
  
  // WebSocket server
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
}

void loop(){
  httpServer.handleClient();
  webSocket.loop();

  if (discreteMode) {
    baseStepper.run();
  } else {
    // Apply deadzone to joystick values
    if (fabs(joy1x) < JOY_DEADZONE) joy1x = 0;
    if (fabs(joy1y) < JOY_DEADZONE) joy1y = 0;
    if (fabs(joy2x) < JOY_DEADZONE) joy2x = 0;
    if (fabs(joy2y) < JOY_DEADZONE) joy2y = 0;
    
    // Stepper control
    baseStepper.setSpeed(joy1x * STEPPER_MAX_SPEED);
    baseStepper.run();
    
    // Servo control with joystick
    shoulderAngle = constrain(shoulderAngle + joy1y * SERVO_STEP_DEG,
                              SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    shoulder.writeMicroseconds(
      map((int)shoulderAngle, 0, (int)SERVO_MAX_ANGLE, 500, 2500)
    );
    
    elbowAngle = constrain(elbowAngle + joy2y * SERVO_STEP_DEG,
                            SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    elbow.writeMicroseconds(
      map((int)elbowAngle, 0, (int)SERVO_MAX_ANGLE, 500, 2500)
    );
    
    wristAngle = constrain(wristAngle + joy2x * SERVO_STEP_DEG,
                            SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    wrist.writeMicroseconds(
      map((int)wristAngle, 0, (int)SERVO_MAX_ANGLE, 500, 2500)
    );
  }

  // Grasper control from controller
  grasperAngle = constrain(remoteForce / 100.0 * SERVO_MAX_ANGLE,
                           SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  grasper.writeMicroseconds(
    map((int)grasperAngle, 0, (int)SERVO_MAX_ANGLE, 500, 2500)
  );

  // Broadcast force reading
  if (millis() - lastForceTime >= FORCE_INTERVAL_MS) {
    lastForceTime = millis();
    float raw = armScale.get_units(5);
    float forceN = raw * ARM_FORCE_CAL;
    StaticJsonDocument<128> doc;
    doc["type"]  = "force";
    doc["motor"] = "grasper_servo";
    doc["force"] = forceN;
    String out; serializeJson(doc, out);
    webSocket.broadcastTXT(out);
  }
}

void handleWebSocketEvent(uint8_t num, WStype_t type,
                          uint8_t * payload, size_t length) {
  if (type != WStype_TEXT) return;
  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, payload, length)) return;
  String t = doc["type"];
  if (t == "move") {
    discreteMode = true;
    String m = doc["motor"], dir = doc["dir"];
    if (m == "base_stepper") {
      float spd = (dir=="cw") ? STEPPER_MAX_SPEED : -STEPPER_MAX_SPEED;
      baseStepper.setSpeed(spd);
    } else {
      Servo* sv; float* ang;
      if      (m=="shoulder_servo"){ sv=&shoulder; ang=&shoulderAngle; }
      else if (m=="elbow_servo")   { sv=&elbow;    ang=&elbowAngle;    }
      else if (m=="wrist_servo")   { sv=&wrist;    ang=&wristAngle;    }
      else return;  // Removed grasper control
      *ang = constrain(*ang + (dir=="cw"? SERVO_STEP_DEG : -SERVO_STEP_DEG),
                      SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
      sv->writeMicroseconds(
        map((int)*ang, 0, (int)SERVO_MAX_ANGLE, 500, 2500)
      );
    }
  }
  else if (t == "stop") {
    if (String(doc["motor"]) == "base_stepper") 
      baseStepper.setSpeed(0);
  }
  else if (t == "joystick") {
    discreteMode = false;
    joy1x = doc["j1x"].as<float>();
    joy1y = doc["j1y"].as<float>();
    joy2x = doc["j2x"].as<float>();
    joy2y = doc["j2y"].as<float>();
  }
  else if (t == "force" && String(doc["motor"])=="grasper_servo") {
    remoteForce = doc["force"].as<float>();
  }
}