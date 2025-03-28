#define BLYNK_TEMPLATE_ID "TMPL6ZLCad5kE"
#define BLYNK_TEMPLATE_NAME "ESP8266"
#define BLYNK_AUTH_TOKEN "XVy7pNOV6Mh-Qv1Pz9y9FxqR2Tt1-phq"

#include <Arduino.h>
#include "utils.h"
#include <Adafruit_Sensor.h>

#include <ESP8266WiFi.h>
char ssid[] = "CNTT-MMT";    
char pass[] = "13572468"; 
#include <BlynkSimpleEsp8266.h>

#include <DHT.h>
#include <Wire.h>
#include <U8g2lib.h>

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define dhtPIN 16     
#define dhtTYPE DHT11 

#define OLED_SDA 13
#define OLED_SCL 12

#include <cstdlib> // Thêm thư viện tạo số ngẫu nhiên

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
DHT dht(dhtPIN, dhtTYPE);

float fHumidity = 0.0;
float fTemperature = 0.0;
ulong startTime = 0; // Lưu thời gian ESP32 bắt đầu chạy
bool yellowBlinkMode = false; // Chế độ đèn vàng nhấp nháy
BlynkTimer timer;


BLYNK_WRITE(V3) {
  yellowBlinkMode = param.asInt();  
}

void GenerateRandomTempHumidity() {
  fTemperature = -40.0 + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (80.0 + 40.0)));
  fHumidity = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 100.0));
}

String GetUptime() {
  ulong elapsed = millis() / 1000; // Chuyển từ ms → giây
  int hours = elapsed / 3600;
  int minutes = (elapsed % 3600) / 60;
  int seconds = elapsed % 60;
  
  char buffer[20];
  sprintf(buffer, "Uptime: %02d:%02d:%02d", hours, minutes, seconds);
  return String(buffer);
}

void SendDataToBlynk() {
  Blynk.virtualWrite(V0, GetUptime());  
  Blynk.virtualWrite(V1, fTemperature);
  Blynk.virtualWrite(V2, fHumidity);
}

void TrafficLightControl() {
  static ulong lastTimer = 0;
  static int state = 0;  
  static const int ledPin[3] = {gPIN, yPIN, rPIN};
  static const int durations[3] = {5000, 2000, 3000}; 

  if (yellowBlinkMode) {
    digitalWrite(gPIN, LOW);
    digitalWrite(rPIN, LOW);
    digitalWrite(yPIN, millis() % 1000 < 500 ? HIGH : LOW);
    return;
  }

  if (!IsReady(lastTimer, durations[state])) return;

  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);
  digitalWrite(ledPin[state], HIGH); 

  GenerateRandomTempHumidity();

  String status;
  int countdown = 0;
  if (state == 0) { status = "Den xanh: "; countdown = 5; }
  else if (state == 1) { status = "Den vang: "; countdown = 2; }
  else { status = "Den do: "; countdown = 3; }

  for (int i = countdown; i >= 1; i--) {
    oled.clearBuffer();
    oled.setFont(u8g2_font_6x10_tf);

    String tempStr = StringFormat("Nhiet do: %.2f C", fTemperature);
    oled.drawUTF8(0, 14, tempStr.c_str());

    String humidStr = StringFormat("Do am: %.2f %%", fHumidity);
    oled.drawUTF8(0, 28, humidStr.c_str());

    String trafficStr = status + String(i) + "s";
    oled.drawUTF8(0, 42, trafficStr.c_str());

    String uptimeStr = GetUptime();
    oled.drawUTF8(0, 56, uptimeStr.c_str());

    oled.sendBuffer();
    delay(1000);
  }

  state = (state + 1) % 3;
}


void setup() {
  Serial.begin(115200);
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);

  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);

  dht.begin();
  Wire.begin(OLED_SDA, OLED_SCL);

  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawUTF8(0, 14, "Van Huynh Tuong An");  
  oled.drawUTF8(0, 28, "Huynh Van Nhan");
  oled.drawUTF8(0, 42, "Nguyen Khanh Phuong");  
  oled.sendBuffer();

  startTime = millis(); // Bắt đầu tính thời gian hoạt động
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, SendDataToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
  TrafficLightControl();
}