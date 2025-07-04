#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include "HX711.h"
#include "main_page.h"

// Wi-Fi credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// HTTP + WebSocket servers
WebServer        httpServer(80);
WebSocketsServer webSocket(81);

// Arm’s load cell (HX711)
#define ARM_DT  18
#define ARM_SCK 19
HX711 armScale;

// Motor pins
#define STEP_PIN 14
#define DIR_PIN  27
#define RST_PIN  12

#define SHOULDER_PIN 32
#define ELBOW_PIN    33
#define WRIST_PIN    25
#define GRASPER_PIN  26

// Motion & force settings
const float STEPPER_MAX_SPEED    = 800.0;
const float STEPPER_ACCELERATION = 400.0;
const float SERVO_STEP_DEG       =   2.0;
const float SERVO_MAX_ANGLE      = 270.0;
const float ARM_CAL              =   1.0;  // N per raw unit
const unsigned long FORCE_INTERVAL = 200;

// State & objects
AccelStepper baseStepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
Servo       shoulder, elbow, wrist, grasper;
unsigned long lastForceTime = 0;

// For controller-mode
bool discreteMode = true;
float joy1x=0, joy1y=0, joy2x=0, joy2y=0;
float remoteForce = 0;

// Servo angles
float shoulderAngle = SERVO_MAX_ANGLE/2,
      elbowAngle    = SERVO_MAX_ANGLE/2,
      wristAngle    = SERVO_MAX_ANGLE/2,
      grasperAngle  = SERVO_MAX_ANGLE/2;

// Forward prototypes
void handleWs(uint8_t, WStype_t, uint8_t*, size_t);
void broadcastForce();

void setup() {
  Serial.begin(115200);
  // Stepper reset
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH);
  baseStepper.setMaxSpeed(STEPPER_MAX_SPEED);
  baseStepper.setAcceleration(STEPPER_ACCELERATION);
  // Servos
  shoulder.attach(SHOULDER_PIN, 500, 2500);
  elbow   .attach(ELBOW_PIN,    500, 2500);
  wrist   .attach(WRIST_PIN,    500, 2500);
  grasper .attach(GRASPER_PIN,  500, 2500);
  // Center servos
  for (Servo* s: {&shoulder,&elbow,&wrist,&grasper}) {
    s->writeMicroseconds(map(int(SERVO_MAX_ANGLE/2),0,int(SERVO_MAX_ANGLE),500,2500));
  }
  // HX711
  armScale.begin(ARM_DT, ARM_SCK);
  armScale.set_scale(1.0);
  armScale.tare();
  // Wi-Fi
  WiFi.begin(ssid,password);
  Serial.print("WiFi ");
  while (WiFi.status()!=WL_CONNECTED) { delay(300); Serial.print('.'); }
  Serial.println(" OK");
  // HTTP UI
  httpServer.on("/", HTTP_GET, [](){
    httpServer.send_P(200,"text/html",MAIN_PAGE);
  });
  httpServer.begin();
  // WebSocket
  webSocket.begin();
  webSocket.onEvent(handleWs);
}

void loop() {
  httpServer.handleClient();
  webSocket.loop();
  if (discreteMode) {
    baseStepper.run();
  } else {
    baseStepper.setSpeed(joy1x * STEPPER_MAX_SPEED);
    baseStepper.run();
    shoulderAngle = constrain(shoulderAngle + joy1y*SERVO_STEP_DEG, 0, SERVO_MAX_ANGLE);
    elbowAngle    = constrain(elbowAngle    + joy2y*SERVO_STEP_DEG, 0, SERVO_MAX_ANGLE);
    wristAngle    = constrain(wristAngle    + joy2x*SERVO_STEP_DEG, 0, SERVO_MAX_ANGLE);
    shoulder.writeMicroseconds(map(int(shoulderAngle),0,int(SERVO_MAX_ANGLE),500,2500));
    elbow   .writeMicroseconds(map(int(elbowAngle),   0,int(SERVO_MAX_ANGLE),500,2500));
    wrist   .writeMicroseconds(map(int(wristAngle),   0,int(SERVO_MAX_ANGLE),500,2500));
  }
  // Grasper from remoteForce
  grasperAngle = constrain(remoteForce/40.0 * SERVO_MAX_ANGLE, 0, SERVO_MAX_ANGLE);
  grasper.writeMicroseconds(map(int(grasperAngle),0,int(SERVO_MAX_ANGLE),500,2500));
  // Broadcast own force
  if (millis() - lastForceTime >= FORCE_INTERVAL) {
    lastForceTime = millis();
    float raw = armScale.get_units(5);
    float fn  = raw * ARM_CAL;
    StaticJsonDocument<128> d;
    d["type"]="force"; d["motor"]="grasper_servo"; d["force"]=fn;
    String o; serializeJson(d,o);
    webSocket.broadcastTXT(o);
  }
}

void handleWs(uint8_t num, WStype_t type, uint8_t* p, size_t len) {
  if (type!=WStype_TEXT) return;
  StaticJsonDocument<256> d;
  if (deserializeJson(d,p,len)) return;
  String t = d["type"];
  if (t=="move") {
    discreteMode=true;
    String m=d["motor"], dir=d["dir"];
    if (m=="base_stepper") {
      baseStepper.setSpeed(dir=="cw"?STEPPER_MAX_SPEED:-STEPPER_MAX_SPEED);
    } else {
      Servo* sv; float* ang;
      if      (m=="shoulder_servo"){sv=&shoulder;ang=&shoulderAngle;}
      else if (m=="elbow_servo")   {sv=&elbow;   ang=&elbowAngle;}
      else if (m=="wrist_servo")   {sv=&wrist;   ang=&wristAngle;}
      else if (m=="grasper_servo") {sv=&grasper; ang=&grasperAngle;}
      *ang = constrain(*ang + (dir=="cw"?SERVO_STEP_DEG:-SERVO_STEP_DEG),
                      0, SERVO_MAX_ANGLE);
      sv->writeMicroseconds(map(int(*ang),0,int(SERVO_MAX_ANGLE),500,2500));
    }
  }
  else if (t=="stop" && String(d["motor"])=="base_stepper") {
    baseStepper.setSpeed(0);
  }
  else if (t=="joystick") {
    discreteMode=false;
    joy1x=d["j1x"].as<float>();
    joy1y=d["j1y"].as<float>();
    joy2x=d["j2x"].as<float>();
    joy2y=d["j2y"].as<float>();
  }
  else if (t=="force" && String(d["motor"])=="grasper_servo") {
    remoteForce = d["force"].as<float>();
  }
}
