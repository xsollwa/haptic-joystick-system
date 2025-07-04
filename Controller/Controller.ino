#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <HX711.h>

// Wi-Fi & WebSocket
const char*  WIFI_SSID = "YOUR_SSID";
const char*  WIFI_PWD  = "YOUR_PASSWORD";
WebSocketsClient ws;
const char*  ARM_HOST  = "arm.local";
const uint16_t ARM_PORT = 81;
const char*  ARM_PATH  = "/ws";

// Joystick pins
const int J1X_PIN = 34, J1Y_PIN = 35;
const int J2X_PIN = 32, J2Y_PIN = 33;

// Bulb pressure sensor
const int BULB_DT  = 19;
const int BULB_SCK = 18;
HX711 bulbScale;

// H-bridge pins
const int H_IN1 = 17;
const int H_IN2 = 16;

// Parameters
const int   ADC_CTR   = 2048;
const int   DEADZONE  = ADC_CTR*5/100;
const float ADC_MAX   = 2048.0;
const int   BUF_LEN   = 5;
const int   SAMPLE_MS = 10;
const float MAX_KPA   = 50.0;  // Calibrate this value

int buf1x[BUF_LEN],buf1y[BUF_LEN],buf2x[BUF_LEN],buf2y[BUF_LEN], bufIdx=0;
float lastArmForce=0;

// Helpers
float avgFilter(int raw, int* buf){
  buf[bufIdx]=raw;
  long sum=0; for(int i=0;i<BUF_LEN;i++) sum+=buf[i];
  return float(sum)/BUF_LEN;
}

// WebSocket events
void onWsEvent(WStype_t type, uint8_t* payload, size_t len){
  if(type==WStype_DISCONNECTED){
    Serial.println("WS disconnected, reconnecting...");
    delay(500);
    ws.begin(ARM_HOST, ARM_PORT, ARM_PATH);
  }
  else if(type==WStype_CONNECTED){
    Serial.println("WS connected to arm");
  }
  else if(type==WStype_TEXT){
    StaticJsonDocument<128> doc;
    if(!deserializeJson(doc,payload,len)){
      if(String(doc["type"])=="force" && String(doc["motor"])=="grasper_servo"){
        lastArmForce = doc["force"].as<float>();
      }
    }
  }
}

void setup(){
  Serial.begin(115200);
  analogReadResolution(12);
  
  // HX711
  bulbScale.begin(BULB_DT, BULB_SCK);
  bulbScale.set_scale(1.0);
  bulbScale.tare();
  
  // H-bridge
  pinMode(H_IN1, OUTPUT);
  pinMode(H_IN2, OUTPUT);
  digitalWrite(H_IN1, LOW);
  digitalWrite(H_IN2, LOW);
  
  // Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  Serial.print("Connecting Wi-Fi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(300);
    Serial.print('.');
  }
  Serial.println(" OK");
  
  // WebSocket client
  ws.begin(ARM_HOST, ARM_PORT, ARM_PATH);
  ws.onEvent(onWsEvent);
}

void loop(){
  ws.loop();
  static unsigned long last = millis();
  if(millis()-last < SAMPLE_MS) return;
  last = millis();

  // 1) Read & filter joysticks
  int r1x = analogRead(J1X_PIN)-ADC_CTR;
  int r1y = analogRead(J1Y_PIN)-ADC_CTR;
  int r2x = analogRead(J2X_PIN)-ADC_CTR;
  int r2y = analogRead(J2Y_PIN)-ADC_CTR;
  r1x = abs(r1x)<DEADZONE?0:r1x;
  r1y = abs(r1y)<DEADZONE?0:r1y;
  r2x = abs(r2x)<DEADZONE?0:r2x;
  r2y = abs(r2y)<DEADZONE?0:r2y;
  float j1x = avgFilter(r1x,buf1x)/ADC_MAX;
  float j1y = avgFilter(r1y,buf1y)/ADC_MAX;
  float j2x = avgFilter(r2x,buf2x)/ADC_MAX;
  float j2y = avgFilter(r2y,buf2y)/ADC_MAX;
  bufIdx = (bufIdx+1)%BUF_LEN;

  // Send joystick data
  StaticJsonDocument<128> jd;
  jd["type"]="joystick";
  jd["j1x"]=j1x; jd["j1y"]=j1y;
  jd["j2x"]=j2x; jd["j2y"]=j2y;
  String js; serializeJson(jd,js);
  ws.sendTXT(js);

  // 2) Bulb pressure → convert to percentage
  float raw = bulbScale.get_units(5);
  float kPa = raw * 1.0; // Calibrate this
  float percent = constrain(kPa / MAX_KPA * 100.0, 0, 100);

  // Send force data
  StaticJsonDocument<128> fd;
  fd["type"]="force";
  fd["motor"]="grasper_servo";
  fd["force"]=percent;  // Send as percentage
  String fs; serializeJson(fd,fs);
  ws.sendTXT(fs);

  // 3) H-bridge control - compare force values directly
  float err = lastArmForce - percent;
  if(err>1.0){
    digitalWrite(H_IN1, HIGH);
    digitalWrite(H_IN2, LOW);
  } else if(err<-1.0){
    digitalWrite(H_IN1, LOW);
    digitalWrite(H_IN2, HIGH);
  } else {
    digitalWrite(H_IN1, LOW);
    digitalWrite(H_IN2, LOW);
  }
}