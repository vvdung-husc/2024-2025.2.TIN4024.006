#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL6w3KmiTV8"
#define BLYNK_TEMPLATE_NAME "ESMART"
#define BLYNK_AUTH_TOKEN "p6lHmQbBwfh1f500utKorT7JQZtQhIdt"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

#define relayPin1 26
#define relayPin2 25
#define relayPin3 33
#define relayPin4 32
#define RELAY1 V1
#define RELAY2 V2
#define RELAY3 V3
#define RELAY4 V4

BLYNK_WRITE(RELAY1){
  int p = param.asInt();
  digitalWrite(relayPin1,p);
}
BLYNK_WRITE(RELAY2){
  int p = param.asInt();
  digitalWrite(relayPin2,p);
}
BLYNK_WRITE(RELAY3){
  int p = param.asInt();
  digitalWrite(relayPin3,p);
}
BLYNK_WRITE(RELAY4){
  int p = param.asInt();
  digitalWrite(relayPin4,p);
}

void setup(){
  Serial.begin(115200);
  pinMode(relayPin1,OUTPUT);
  pinMode(relayPin2,OUTPUT);
  pinMode(relayPin3,OUTPUT);
  pinMode(relayPin4,OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop(){
  Blynk.run();
}