#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <HX711.h>

// Wi-Fi & WS client
const char*  WIFI_SSID = "YOUR_SSID";
const char*  WIFI_PWD  = "YOUR_PASSWORD";
WebSocketsClient ws;
const char*  ARM_HOST  = "arm.local";
const uint16_t ARM_PORT = 81;
const char*  ARM_PATH  = "/ws";

// Joysticks (12-bit ADC)
const int J1X=34,J1Y=35,J2X=32,J2Y=33;
const int ADC_CTR=2048, DEADZONE=ADC_CTR*5/100;
const float ADC_MAX=2048.0;
const int BUF_LEN=5, SAMPLE_MS=10;
int buf1x[BUF_LEN],buf1y[BUF_LEN],buf2x[BUF_LEN],buf2y[BUF_LEN],bufIdx=0;
float lastArmForce=0;

// HX710B for bulb pressure
const int BULB_DT=19,BULB_SCK=18;
HX711 bulbScale;

float avgFilter(int raw,int*buf){
  buf[bufIdx]=raw;
  long s=0; for(int i=0;i<BUF_LEN;i++) s+=buf[i];
  return float(s)/BUF_LEN;
}

// H-bridge for actuator
const int H1=17,H2=16;

// WS events
void onWsEvent(WStype_t t, uint8_t* d, size_t l){
  if(t==WStype_DISCONNECTED){
    Serial.println("WS down, reconnecting…");
    delay(500);
    ws.begin(ARM_HOST,ARM_PORT,ARM_PATH);
  }
  else if(t==WStype_CONNECTED){
    Serial.println("WS to arm OK");
  }
  else if(t==WStype_TEXT){
    StaticJsonDocument<128> doc;
    if(!deserializeJson(doc,d,l) && String(doc["type"])=="force" &&
       String(doc["motor"])=="grasper_servo"){
      lastArmForce = doc["force"].as<float>();
    }
  }
}

void setup(){
  Serial.begin(115200);
  analogReadResolution(12);
  // bulb HX711
  bulbScale.begin(BULB_DT,BULB_SCK);
  bulbScale.set_scale(1.0);
  bulbScale.tare();
  // H-bridge pins
  pinMode(H1,OUTPUT); pinMode(H2,OUTPUT);
  digitalWrite(H1,LOW); digitalWrite(H2,LOW);
  // Wi-Fi
  WiFi.begin(WIFI_SSID,WIFI_PWD);
  Serial.print("WiFi…");
  while(WiFi.status()!=WL_CONNECTED){
    delay(300); Serial.print('.');
  }
  Serial.println(" OK");
  // WS client
  ws.begin(ARM_HOST,ARM_PORT,ARM_PATH);
  ws.onEvent(onWsEvent);
}

void loop(){
  ws.loop();
  static unsigned long last=millis();
  if(millis()-last<SAMPLE_MS) return;
  last=millis();

  // read joysticks
  int r1x=analogRead(J1X)-ADC_CTR,
      r1y=analogRead(J1Y)-ADC_CTR,
      r2x=analogRead(J2X)-ADC_CTR,
      r2y=analogRead(J2Y)-ADC_CTR;
  r1x=abs(r1x)<DEADZONE?0:r1x;
  r1y=abs(r1y)<DEADZONE?0:r1y;
  r2x=abs(r2x)<DEADZONE?0:r2x;
  r2y=abs(r2y)<DEADZONE?0:r2y;
  float j1x=avgFilter(r1x,buf1x)/ADC_MAX,
        j1y=avgFilter(r1y,buf1y)/ADC_MAX,
        j2x=avgFilter(r2x,buf2x)/ADC_MAX,
        j2y=avgFilter(r2y,buf2y)/ADC_MAX;
  bufIdx=(bufIdx+1)%BUF_LEN;
  // send joystick JSON
  StaticJsonDocument<128> jd;
  jd["type"]="joystick";
  jd["j1x"]=j1x; jd["j1y"]=j1y;
  jd["j2x"]=j2x; jd["j2y"]=j2y;
  String out; serializeJson(jd,out);
  ws.sendTXT(out);

  // read bulb pressure & send
  float raw=bulbScale.get_units(5);
  float kPa=raw*1.0;  // calibrate later
  StaticJsonDocument<128> fd;
  fd["type"]="force";
  fd["motor"]="grasper_servo";
  fd["force"]=kPa;
  String fo; serializeJson(fd,fo);
  ws.sendTXT(fo);

  // match arm’s force via H-bridge
  float err=lastArmForce-kPa;
  if(err>1.0){
    digitalWrite(H1,HIGH);
    digitalWrite(H2,LOW);
  } else if(err<-1.0){
    digitalWrite(H1,LOW);
    digitalWrite(H2,HIGH);
  } else {
    digitalWrite(H1,LOW);
    digitalWrite(H2,LOW);
  }
}
