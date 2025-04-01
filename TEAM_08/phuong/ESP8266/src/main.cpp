#include <Arduino.h>
#include "utils.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <Wire.h>
#include <U8g2lib.h>

#include <stdlib.h>

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define OLED_SDA 13
#define OLED_SCL 12

// Khởi tạo OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool welcomeDisplayed = false; // Biến kiểm tra đã hiển thị thông điệp chào mừng
void DisplayMessage();
void setup() {
  Serial.begin(115200);
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);  // SDA, SCL

  oled.begin();

  // Hiển thị thông điệp chào mừng 2 giây đầu
  DisplayMessage();
  delay(2000);
  oled.clearBuffer();
  oled.sendBuffer();

  welcomeDisplayed = true; // Đánh dấu là đã hiển thị xong thông điệp chào mừng
}

void ThreeLedBlink() {
  static ulong lastTimer = 0;
  static int currentLed = 0;  
  static const int ledPin[3] = {gPIN, yPIN, rPIN};

  if (!IsReady(lastTimer, 1000)) return;
  int prevLed = (currentLed + 2) % 3;
  digitalWrite(ledPin[prevLed], LOW);  
  digitalWrite(ledPin[currentLed], HIGH);  
  currentLed = (currentLed + 1) % 3;
}

void DisplayMessage() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Từn En");  
  oled.drawUTF8(0, 28, "khoa CNTT");
  oled.drawUTF8(0, 42, "chơi Elsu");
  oled.drawUTF8(0, 54, "bị lỏ");
  oled.sendBuffer();
}

void DisplayTemperatureHumidity() {
  float t = random(200, 350) / 10.0; // Nhiệt độ ngẫu nhiên từ 20.0 đến 35.0
  float h = random(400, 800) / 10.0; // Độ ẩm ngẫu nhiên từ 40.0 đến 80.0
  ulong runtime = millis() / 1000; // Thời gian chạy tính bằng giây
  
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  
  String s = StringFormat("Temperature: %.1f °C", t);
  oled.drawUTF8(0, 14, s.c_str());  
  
  s = StringFormat("Humidity: %.1f %%", h);
  oled.drawUTF8(0, 28, s.c_str());  
  
  s = StringFormat("Runtime: %lu s", runtime);
  oled.drawUTF8(0, 42, s.c_str());      
  
  oled.sendBuffer();
}

void loop() {
  ThreeLedBlink();
  
  static ulong lastTimer = 0;
  if (!IsReady(lastTimer, 1000)) return;
  
  DisplayTemperatureHumidity(); // Luôn hiển thị nhiệt độ & độ ẩm sau 2s đầu tiên
}
