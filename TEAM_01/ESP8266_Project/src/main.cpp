#include <Arduino.h>
#include "utils.h"
#include <Wire.h>
#include <U8g2lib.h>


// Lê Hữu Nhật
#define BLYNK_TEMPLATE_ID "TMPL6c_gqr655"
#define BLYNK_TEMPLATE_NAME "ESP8266 Project"
#define BLYNK_AUTH_TOKEN "rU27I2d9cKLS0k-CgXPqnN1Tca0ixqid"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Thông tin WiFi
char ssid[] = "CNTT-MMT";
char pass[] = "13572468";

#define gPIN 15  // Đèn xanh
#define yPIN 2   // Đèn vàng
#define rPIN 5   // Đèn đỏ

#define OLED_SDA 13
#define OLED_SCL 12

U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool yellowBlinkMode = false;

int currentLedIndex = 0;
unsigned long lastLedSwitchTime = 0;
const int ledPin[3] = {gPIN, yPIN, rPIN};
const int durations[3] = {5000, 7000, 2000}; // Xanh 5s, Vàng 7s, Đỏ 2s

bool WelcomeDisplayTimeout(uint msSleep = 5000){
  static unsigned long lastTimer = 0;
  static bool bDone = false;
  if (bDone) return true;
  if (!IsReady(lastTimer, msSleep)) return false;
  bDone = true;
  return bDone;
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Trường ĐHKH");  
  oled.drawUTF8(0, 28, "Khoa CNTT");
  oled.drawUTF8(0, 42, "Lập trình IoT");  
  oled.sendBuffer();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  if (Blynk.connected()) {
    Serial.println("Connected to Blynk!");
  } else {
    Serial.println("Failed to connect to Blynk!");
  }
}

void ThreeLedBlink(){
  static unsigned long lastTimer = 0;

  if (yellowBlinkMode) return;

  if (!IsReady(lastTimer, durations[currentLedIndex])) return;

  int prevLed = (currentLedIndex + 2) % 3;
  digitalWrite(ledPin[prevLed], LOW);  
  digitalWrite(ledPin[currentLedIndex], HIGH);  
  lastLedSwitchTime = millis(); // Cập nhật thời điểm chuyển đèn
  currentLedIndex = (currentLedIndex + 1) % 3;
}

void yellowBlink() {
  static unsigned long lastTimer = 0;
  static bool state = false;

  if (!yellowBlinkMode) return;

  if (!IsReady(lastTimer, 2000)) return;
  
  state = !state;
  digitalWrite(gPIN, LOW);
  digitalWrite(rPIN, LOW);
  digitalWrite(yPIN, state);
}

float generateRandomTemperature() {
  return random(-400, 801) / 10.0;
}

float generateRandomHumidity() {
  return random(0, 1001) / 10.0;
}

void updateRandomDHT(){
  static unsigned long lastTimer = 0;  
  if (!IsReady(lastTimer, 1000)) return; // Cập nhật nhiệt độ, độ ẩm mỗi 5 giây

  float t = generateRandomTemperature();
  float h = generateRandomHumidity();

  Serial.print("Random Temperature: ");
  Serial.print(t);
  Serial.println(" *C");
  Serial.print("Random Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);

  // Hiển thị trên OLED
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1); // Dùng font nhỏ hơn để vừa 4 dòng
  String tempStr = StringFormat("Nhiet: %.1f C", t);
  String humStr = StringFormat("Do am: %.1f %%", h);

  unsigned long uptime = millis() / 1000;
  int hours = uptime / 3600;
  int minutes = (uptime % 3600) / 60;
  int seconds = uptime % 60;
  String uptimeStr = StringFormat("Up: %dh %02dm %02ds", hours, minutes, seconds);
  
  oled.drawUTF8(0, 14, tempStr.c_str());  // Dòng 1
  oled.drawUTF8(0, 28, humStr.c_str());   // Dòng 2
  oled.drawUTF8(0, 42, uptimeStr.c_str()); // Dòng 3
  
  if (!yellowBlinkMode) {
    unsigned long elapsed = millis() - lastLedSwitchTime;
    int remainingTime = (durations[currentLedIndex] - elapsed) / 1000; // Giây còn lại
    if (remainingTime < 0) remainingTime = 0; // Đảm bảo không âm
    
    String ledStr;
    if (ledPin[currentLedIndex] == gPIN) ledStr = "Do";
    else if (ledPin[currentLedIndex] == yPIN) ledStr = "Xanh";
    else ledStr = "Vang";
    
    String countdownStr = StringFormat("%s: %ds", ledStr.c_str(), remainingTime);
    oled.drawUTF8(0, 56, countdownStr.c_str()); // Dòng 4
  }

  oled.sendBuffer();
}

void updateUptime() {
  static unsigned long lastTimer = 0;
  if (!IsReady(lastTimer, 1000)) return; // Cập nhật mỗi 1 giây
  
  unsigned long uptime = millis() / 1000; // Thời gian chạy tính bằng giây
  Blynk.virtualWrite(V0, uptime);
  
  Serial.print("Uptime (seconds) sent to Blynk: ");
  Serial.println(uptime);
}

BLYNK_WRITE(V3) {
  yellowBlinkMode = param.asInt();
  if (!yellowBlinkMode) {
    digitalWrite(yPIN, LOW);
  }
}

void loop() {
  Blynk.run();
  if (!WelcomeDisplayTimeout()) return;
  ThreeLedBlink();
  yellowBlink();
  updateRandomDHT();
  updateUptime();
}