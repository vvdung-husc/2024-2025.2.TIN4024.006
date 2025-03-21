#define BLYNK_TEMPLATE_ID "TMPL6RQGDLOQe" 
#define BLYNK_TEMPLATE_NAME "ESP8266" 
#define BLYNK_AUTH_TOKEN "HkHpsRJ8SZ-wbZcBynAWmAqATjJOvHQV" 

#include <Arduino.h>
#include "utils.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <U8g2lib.h>

// Định nghĩa chân kết nối
#define gPIN 15
#define yPIN 2
#define rPIN 5

#define OLED_SDA 13
#define OLED_SCL 12

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool blinkMode = false;

unsigned long upTime = 0;
float fTemperature = 0.0;
float fHumidity = 0.0;

BlynkTimer timer;

BLYNK_WRITE(V3) {
  blinkMode = param.asInt();
}

bool WelcomeDisplayTimeout(unsigned int msSleep = 3000) {
  static unsigned long lastTimer = 0;
  static bool bDone = false;
  if (bDone) return true;
  if (millis() - lastTimer < msSleep) return false;
  bDone = true;
  return bDone;
}

//Hiển thị trên OLED
void updateOLED() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese2);

  oled.drawUTF8(0, 14, ("Nhiet do: " + String(fTemperature, 1) + "°C").c_str());
  oled.drawUTF8(0, 28, ("Do am: " + String(fHumidity, 1) + "%").c_str());
  oled.drawUTF8(0, 42, ("Uptime: " + String(upTime) + "s").c_str());

  oled.sendBuffer();
}

//Điều khiển đèn giao thông
void TrafficLightControl() {
  static unsigned long lastTimer = 0;
  static int state = 0;
  static const unsigned long durations[] = {2000, 3000, 1000}; // Đỏ 10s, Xanh 8s, Vàng 3s
  static const int ledPins[] = {rPIN, gPIN, yPIN};

  if (blinkMode) {
    // Trạng thái bật/tắt nhấp nháy đèn vàng
    if (millis() - lastTimer > 500) {
      lastTimer = millis();
      digitalWrite(yPIN, !digitalRead(yPIN));
    }
    digitalWrite(rPIN, LOW);
    digitalWrite(gPIN, LOW);
    return;
  }

  if (millis() - lastTimer > durations[state]) {
    lastTimer = millis();
    digitalWrite(ledPins[state], LOW);
    state = (state + 1) % 3;
    digitalWrite(ledPins[state], HIGH);
  }
}


// Gửi dữ liệu lên Blynk
void sendToBlynk() {
  Blynk.virtualWrite(V0, upTime);      // Thời gian chạy
  Blynk.virtualWrite(V1, fTemperature); // Nhiệt độ
  Blynk.virtualWrite(V2, fHumidity);    // Độ ẩm
}

// Sinh dữ liệu nhiệt độ & độ ẩm ngẫu nhiên
float randomTemperature() {
  return random(-400, 800) / 10.0;
}

float randomHumidity() {
  return random(0, 1000) / 10.0;
}


// Cập nhật nhiệt độ & độ ẩm
void updateSensorData() {
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 2000) return;
  lastTimer = millis();

  fTemperature = randomTemperature();
  fHumidity = randomHumidity();

  Serial.print("Nhiet do: ");
  Serial.print(fTemperature);
  Serial.println("°C");

  Serial.print("Do am: ");
  Serial.print(fHumidity);
  Serial.println("%");
}

// Hiển thị thời gian chạy
void updateupTime() {
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 1000) return;
  lastTimer = millis();

  upTime++;
}

// SETUP
void setup() {
  Serial.begin(115200);

  //Cấu hình LED
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, HIGH); // Bắt đầu với đèn đỏ

  // Cấu hình OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Trường ĐHKH");
  oled.drawUTF8(0, 28, "Khoa CNTT");
  oled.drawUTF8(0, 42, "Lập trình IoT");
  oled.sendBuffer();
  oled.sendBuffer();

  // Kết nối WiFi & Blynk
  const char* ssid = "CNTT-MMT";       // Thay bằng SSID thực tế
  const char* password = "13572468";   // Thay bằng mật khẩu thực tế

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Gửi dữ liệu lên Blynk mỗi 3 giây
  timer.setInterval(3000L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();

  if (!WelcomeDisplayTimeout()) return;

  TrafficLightControl();
  updateSensorData();
  updateupTime();
  updateOLED();
}