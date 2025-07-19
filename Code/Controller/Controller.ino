#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <HX711.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi & Ws client
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PWD  = "YOUR_PASSWORD";
WebSocketsClient ws;
const char* ARM_HOST  = "arm.local";
const uint16_t ARM_PORT = 81;
const char* ARM_PATH  = "/ws";

// Joysticks
const int J1X = 34, J1Y = 35, J2X = 32, J2Y = 33;
const int J1_BTN = 23, J2_BTN = 22;
const int BUF_LEN = 5, SAMPLE_MS = 10;
int buf1x[BUF_LEN] = {}, buf1y[BUF_LEN] = {}, buf2x[BUF_LEN] = {}, buf2y[BUF_LEN] = {};
int bufIdx = 0;
int ctr1x = 2048, ctr1y = 2048, ctr2x = 2048, ctr2y = 2048;  // Midpoints
const float ADC_MAX = 2048.0;
volatile float lastArmForce = 0;
const int DEADZONE_PCT = 5;
int DEADZONE = ADC_MAX * DEADZONE_PCT / 100;

// HX711 for bulb pressure
const int BULB_DT = 19, BULB_SCK = 18;
HX711 bulbScale;

// H-bridge pins for actuator
const int H1 = 17, H2 = 16;

// OLED 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int screenPage = 0;
const int NUM_PAGES = 4;

// WS reconnect flag
bool wsReconnectNeeded = false;

// Moving average filter
float avgFilter(int raw, int* buf) {
  buf[bufIdx] = raw;
  long sum = 0;
  for (int i = 0; i < BUF_LEN; i++) sum += buf[i];
  return float(sum) / BUF_LEN;
}

// WS events
void onWsEvent(WStype_t type, uint8_t* data, size_t len) {
  if (type == WStype_DISCONNECTED) {
    Serial.println("WebSocket disconnected, reconnecting...");
    wsReconnectNeeded = true;
  } else if (type == WStype_CONNECTED) {
    Serial.println("WS to arm OK");
  } else if (type == WStype_TEXT) {
    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, data, len) &&
        String(doc["type"]) == "force" &&
        String(doc["motor"]) == "grasper_servo") {
      lastArmForce = doc["force"].as<float>();
    }
  }
}

void calibrateJoysticks() {
  ctr1x = analogRead(J1X);
  ctr1y = analogRead(J1Y);
  ctr2x = analogRead(J2X);
  ctr2y = analogRead(J2Y);
  Serial.printf("Joystick centers calibrated: J1(%d,%d), J2(%d,%d)\n", ctr1x, ctr1y, ctr2x, ctr2y);
}

void updateDisplay(float j1x, float j1y, float j2x, float j2y, float kPa) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  switch (screenPage) {
    case 0:
      display.setTextSize(1);
      display.println("Arm Force:");
      display.setTextSize(2);
      display.print(lastArmForce, 2); display.println(" N");
      break;
    case 1:
      display.setTextSize(1);
      display.println("Bulb Pressure:");
      display.setTextSize(2);
      display.print(kPa, 2); display.println(" kPa");
      break;
    case 2:
      display.setTextSize(1);
      display.println("Joystick Values:");
      display.printf("J1 X: %.2f\n", j1x);
      display.printf("J1 Y: %.2f\n", j1y);
      display.printf("J2 X: %.2f\n", j2x);
      display.printf("J2 Y: %.2f\n", j2y);
      break;
    case 3:
      display.setTextSize(1);
      display.print("SSID: "); display.println(WiFi.SSID());
      display.print("IP: "); display.println(WiFi.localIP());
      display.print("WS: "); display.println(ws.isConnected() ? "Connected" : "Lost");
      break;
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(J1_BTN, INPUT_PULLUP);
  pinMode(J2_BTN, INPUT_PULLUP);

  // H-bridge setup
  pinMode(H1, OUTPUT); pinMode(H2, OUTPUT);
  digitalWrite(H1, LOW); digitalWrite(H2, LOW);

   if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (1);
  }

  // HX711 setup
  bulbScale.begin(BULB_DT, BULB_SCK);
  bulbScale.set_scale(1.0); 
  bulbScale.tare();

  // Wi-Fi connect
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300); Serial.print('.');
  }
  Serial.println(" connected");

  // Calibrate joystick 
  calibrateJoysticks();
  ws.begin(ARM_HOST, ARM_PORT, ARM_PATH);
  ws.onEvent(onWsEvent);
}

void loop() {

  static bool j1Prev = HIGH, j2Prev = HIGH;
  bool j1Now = digitalRead(J1_BTN);
  bool j2Now = digitalRead(J2_BTN);
  if (j1Prev == HIGH && j1Now == LOW) screenPage = (screenPage + 1) % NUM_PAGES;
  if (j2Prev == HIGH && j2Now == LOW) screenPage = (screenPage - 1 + NUM_PAGES) % NUM_PAGES;
  j1Prev = j1Now;
  j2Prev = j2Now;

  // Reconnect Wi-Fi 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost, reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    delay(500);
    return;
  }

  // Reconnect WebSocket 
  if (wsReconnectNeeded) {
    wsReconnectNeeded = false;
    ws.begin(ARM_HOST, ARM_PORT, ARM_PATH);
    ws.onEvent(onWsEvent);
  }

  ws.loop();

  static unsigned long last = millis();
  if (millis() - last < SAMPLE_MS) return;
  last = millis();

  // read joysticks
  int r1x = analogRead(J1X) - ctr1x;
  int r1y = analogRead(J1Y) - ctr1y;
  int r2x = analogRead(J2X) - ctr2x;
  int r2y = analogRead(J2Y) - ctr2y;

  r1x = abs(r1x) < DEADZONE ? 0 : r1x;
  r1y = abs(r1y) < DEADZONE ? 0 : r1y;
  r2x = abs(r2x) < DEADZONE ? 0 : r2x;
  r2y = abs(r2y) < DEADZONE ? 0 : r2y;

  float j1x = avgFilter(r1x, buf1x) / ADC_MAX;
  float j1y = avgFilter(r1y, buf1y) / ADC_MAX;
  float j2x = avgFilter(r2x, buf2x) / ADC_MAX;
  float j2y = avgFilter(r2y, buf2y) / ADC_MAX;
  bufIdx = (bufIdx + 1) % BUF_LEN;

  // send joystick JSON
  StaticJsonDocument<128> jd;
  jd["type"] = "joystick";
  jd["j1x"] = j1x;
  jd["j1y"] = j1y;
  jd["j2x"] = j2x;
  jd["j2y"] = j2y;
  String out;
  serializeJson(jd, out);
  ws.sendTXT(out);

  // read & send pressure data
  float raw = bulbScale.get_units(5);
  float kPa = raw * 1.0; // Replace with calibrated multiplier
  StaticJsonDocument<128> fd;
  fd["type"] = "force";
  fd["motor"] = "grasper_servo";
  fd["force"] = kPa;
  String fo;
  serializeJson(fd, fo);
  ws.sendTXT(fo);

  // match arm’s force via H-bridge
   float err = lastArmForce - kPa;
  if (err > 1.0) {
    digitalWrite(H1, HIGH); digitalWrite(H2, LOW);
  } else if (err < -1.0) {
    digitalWrite(H1, LOW); digitalWrite(H2, HIGH);
  } else {
    digitalWrite(H1, LOW); digitalWrite(H2, LOW);
  }

  updateDisplay(j1x, j1y, j2x, j2y, kPa);
}
