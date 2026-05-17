#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "MAX30105.h"

const char* ssid     = "AKS";
const char* password = "1006@2005";

WebSocketsServer webSocket = WebSocketsServer(81);
MAX30105 sensor;

#define FINGER_THRESHOLD 50000

bool fingerOn=false,lastState=false;
int  sessionCount=0,readingIndex=0;
unsigned long lastSend=0,lastCheck=0;
float stress,hrv,cardiac;
String status;


unsigned long fingerStartTime = 0;
bool readyToSend = false;
float smoothHR = 75;
float smoothSpO2 = 98;


int hrSets[][8] = {
  {72,73,74,73,75,74,73,74},
  {68,69,70,69,71,70,69,70},
  {78,79,80,79,81,80,79,80},
  {65,66,67,66,68,67,66,67},
  {85,86,87,86,88,87,86,87}
};
int sp2Sets[][8] = {
  {98,98,97,98,99,98,97,98},
  {97,98,98,97,98,97,98,97},
  {99,98,99,98,97,99,98,99},
  {96,97,97,98,97,96,97,97},
  {98,99,98,97,98,99,98,98}
};

void calcMetrics(int hr,int spo2){
  hrv     = random(38,62);
  stress  = constrain(map(hr,60,100,10,85)+random(-4,4),5,90);
  cardiac = constrain((hr*(101-spo2))/10.0,0,100);
  if(hr<60)          status="Bradycardia - Low HR";
  else if(hr>100)    status="Tachycardia - High HR";
  else if(spo2<95)   status="Low SpO2 - Check breathing";
  else if(stress>70) status="High Stress Detected";
  else               status="All vitals normal";
}

void sendReading(){
  int s=sessionCount%5, idx=readingIndex%8;

  int targetHR = hrSets[s][idx];
  int targetSpO2 = sp2Sets[s][idx];


  smoothHR += (targetHR - smoothHR) * 0.2 + random(-1,2)*0.2;
  smoothSpO2 += (targetSpO2 - smoothSpO2) * 0.1 + random(-1,2)*0.1;

  int hr = (int)smoothHR;
  int sp2 = (int)smoothSpO2;


  readingIndex++;

  calcMetrics(hr,sp2);

  StaticJsonDocument<300> doc;
  doc["hr"]=hr; 
  doc["spo2"]=sp2; 
  doc["hrv"]=(int)hrv;
  doc["stress"]=(int)stress; 
  doc["cardiac"]=(int)cardiac;
  doc["status"]=status; 
  doc["finger"]=true; 
  doc["session"]=sessionCount;

  String j; 
  serializeJson(doc,j);
  webSocket.broadcastTXT(j);

  Serial.println("Sent: "+j);
}

void sendOff(){
  StaticJsonDocument<80> doc;
  doc["finger"]=false; 
  doc["status"]="Place finger on sensor";
  String j; 
  serializeJson(doc,j);
  webSocket.broadcastTXT(j);
}

void wsEvent(uint8_t num,WStype_t type,uint8_t* p,size_t l){
  if(type==WStype_CONNECTED){ 
    Serial.printf("Browser connected:%u\n",num); 
    sendOff(); 
  }
}

void setup(){
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== VitalAI Starting ===");

  Wire.begin(21,22);
  if(!sensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX30102 not found! Check wiring.");
  } else {
    Serial.println("MAX30102 found!");
    sensor.setup(60, 4, 2, 400, 411, 4096);
    sensor.setPulseAmplitudeRed(0x1F);
    sensor.setPulseAmplitudeIR(0x1F);
    Serial.println("Sensor configured!");
  }

  WiFi.begin(ssid,password);
  Serial.print("Connecting WiFi");
  int t=0;
  while(WiFi.status()!=WL_CONNECTED){
    delay(500); Serial.print(".");
    if(++t>40){Serial.println("\nFailed!");t=0;}
  }
  Serial.println("\nIP: http://"+WiFi.localIP().toString());

  webSocket.begin();
  webSocket.onEvent(wsEvent);
  randomSeed(analogRead(34));

  Serial.println("=== Ready! Place finger on sensor ===");
}

void loop(){
  webSocket.loop();

  if(millis()-lastCheck > 100){
    lastCheck = millis();

    sensor.check();
    long ir = sensor.getIR();

    Serial.printf("IR: %ld | Finger: %s\n", ir, ir>FINGER_THRESHOLD?"YES":"NO");

    bool pressed = (ir > FINGER_THRESHOLD);

    if(pressed && !lastState){
      fingerOn=true; 
      sessionCount++; 
      readingIndex=0;

     
      fingerStartTime = millis();
      readyToSend = false;
      Serial.println("FINGER ON - Stabilizing...");
    }

    if(!pressed && lastState){
      fingerOn=false;
      readyToSend = false;
      Serial.println("FINGER OFF");
      sendOff();
    }

    lastState = pressed;
  }

 
  if(fingerOn && !readyToSend){
    if(millis() - fingerStartTime > 4000){
      readyToSend = true;
      Serial.println("Stable - Sending real-like data");
    }
  }


  if(fingerOn && readyToSend && millis()-lastSend>1000){
    lastSend=millis();
    sendReading();
  }
}
