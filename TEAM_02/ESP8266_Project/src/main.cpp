#include <Arduino.h>
#include "utils.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID "TMPL6vCrMNkel"
#define BLYNK_TEMPLATE_NAME "ESP8266 Project"
#define BLYNK_AUTH_TOKEN "CsgSkLzpxCu1uhHhnHlwJu1JkGXGkSSF"

char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define dhtPIN 16     // Digital pin connected to the DHT sensor
#define dhtTYPE DHT11 // DHT 22 (AM2302)

#define OLED_SDA 13
#define OLED_SCL 12

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
DHT dht(D0, dhtTYPE);

BlynkTimer timer;
bool blinkMode = false;
unsigned long startTime;

BLYNK_WRITE(V1) {
    blinkMode = param.asInt();
}

void updateSensors() {
    float temperature = random(-400, 800) / 10.0;
    float humidity = random(0, 1000) / 10.0;
    
    Blynk.virtualWrite(V2, temperature);
    Blynk.virtualWrite(V3, humidity);

    oled.clearBuffer();
    oled.setFont(u8g2_font_ncenB08_tr);
    oled.setCursor(0, 10);
    oled.print("Temp: ");
    oled.print(temperature);
    oled.print(" C");
    oled.setCursor(0, 25);
    oled.print("Humidity: ");
    oled.print(humidity);
    oled.print(" %");
    oled.sendBuffer();
}

void updateLEDs() {
    if (blinkMode) {
        digitalWrite(gPIN, LOW);
        digitalWrite(rPIN, LOW);
        digitalWrite(yPIN, millis() % 1000 < 500 ? HIGH : LOW);
    } else {
        digitalWrite(gPIN, HIGH);
        delay(3000);
        digitalWrite(gPIN, LOW);
        digitalWrite(yPIN, HIGH);
        delay(1000);
        digitalWrite(yPIN, LOW);
        digitalWrite(rPIN, HIGH);
        delay(3000);
        digitalWrite(rPIN, LOW);
    }
}

void updateUptime() {
    Blynk.virtualWrite(V0, (millis() - startTime) / 1000);
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    
    pinMode(gPIN, OUTPUT);
    pinMode(yPIN, OUTPUT);
    pinMode(rPIN, OUTPUT);
    
    dht.begin();
    oled.begin();
    oled.clearBuffer();
    oled.sendBuffer();
    
    startTime = millis();
    
    timer.setInterval(2000L, updateSensors);
    timer.setInterval(100L, updateLEDs);
    timer.setInterval(1000L, updateUptime);
}

void loop() {
    Blynk.run();
    timer.run();
}
