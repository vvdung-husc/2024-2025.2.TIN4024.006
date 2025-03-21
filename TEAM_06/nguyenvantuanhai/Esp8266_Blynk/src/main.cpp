#include <Arduino.h>
#include "utils.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <Wire.h>
#include <U8g2lib.h>

// Nguyễn Văn Tuấn Hải
#define BLYNK_TEMPLATE_ID "TMPL6_0wllQ2P"
#define BLYNK_TEMPLATE_NAME "ESP8266 Blynk"
#define BLYNK_AUTH_TOKEN "8_dbofoXgl9tGvigYjj_oDOM6EuZOJfC"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>

#define gPIN 15
#define yPIN 2
#define rPIN 5

#define dhtPIN 16     // Digital pin connected to the DHT sensor
#define dhtTYPE DHT11 // DHT 11 sensor

#define OLED_SDA 13
#define OLED_SCL 12

// Khởi tạo màn hình OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

DHT dht(D0, dhtTYPE);

// WiFi credentials
char ssid[] = "CNTT-MMT";
char pass[] = "13572468";

bool buttonState = false;

bool WelcomeDisplayTimeout(uint msSleep = 5000) {
    static ulong lastTimer = 0;
    static bool bDone = false;
    if (bDone) return true;
    if (!IsReady(lastTimer, msSleep)) return false;
    bDone = true;
    return bDone;
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

    Wire.begin(OLED_SDA, OLED_SCL); // SDA, SCL
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi connection failed!");
    } else {
        Serial.println("✅ WiFi connected!");
    }
    Blynk.virtualWrite(V3, buttonState);
    oled.begin();
    oled.clearBuffer();
    oled.sendBuffer();
    randomSeed(analogRead(0));
}

void ThreeLedBlink() {
    static ulong lastTimer = 0;
    static int currentLed = 0;
    static const int ledPin[3] = { gPIN, yPIN, rPIN };

    if (!IsReady(lastTimer, 1000)) return;
    int prevLed = (currentLed + 2) % 3;
    digitalWrite(ledPin[prevLed], LOW);
    digitalWrite(ledPin[currentLed], HIGH);
    currentLed = (currentLed + 1) % 3;
}

float fHumidity = 0.0;
float fTemperature = 0.0;

void updateDisplay() {
  static ulong lastTimer = 0;
  if (!IsReady(lastTimer, 1000)) return; // Cập nhật mỗi giây

  ulong uptimeSeconds = millis() / 1000;
  int minutes = (uptimeSeconds % 3600) / 60;
  int seconds = uptimeSeconds % 60;

  float h = random(0, 10001) / 100.0;    // Giả lập dữ liệu độ ẩm
  float t = random(-4000, 8001) / 100.0; // Giả lập dữ liệu nhiệt độ

  if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
  }

  // Cập nhật giá trị
  fTemperature = t;
  fHumidity = h;

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x13B_tf); // Font nhỏ, hiển thị rõ 3 dòng

  // Dòng 1: Nhiệt độ
  String tempStr = StringFormat("Nhiet do: %.1f C", t);
  oled.drawUTF8(0, 16, tempStr.c_str());

  // Dòng 2: Độ ẩm
  String humStr = StringFormat("Do am: %.1f %%", h);
  oled.drawUTF8(0, 32, humStr.c_str());

  // Dòng 3: Uptime
  String timeStr = StringFormat("Thoi gian: %02d:%02d", minutes, seconds);
  oled.drawUTF8(0, 48, timeStr.c_str());

  oled.sendBuffer();

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V0, uptimeSeconds);
}

// Hiển thị thời gian hoạt động (uptime) trên OLED
void DrawUptime() {
    static ulong lastTimer = 0;
    if (!IsReady(lastTimer, 1000)) return; // Cập nhật mỗi giây

    ulong uptimeSeconds = millis() / 1000; // Lấy thời gian uptime (giây)
    int hours = uptimeSeconds / 3600;
    int minutes = (uptimeSeconds % 3600) / 60;
    int seconds = uptimeSeconds % 60;

    oled.clearBuffer();
    oled.setFont(u8g2_font_unifont_t_vietnamese2);

    // Hiển thị thời gian uptime
    String timeStr = StringFormat("Thoi gian: %02d:%02d", minutes, seconds);
    oled.drawUTF8(0, 14, timeStr.c_str());

    oled.sendBuffer();
}

void BlinkingYellowLight() {
    static bool yellowState = false;
    static unsigned long previousBlinkMillis = 0;
    if (IsReady(previousBlinkMillis, 500)) {
        yellowState = !yellowState;
        digitalWrite(yPIN, yellowState);
    }

    digitalWrite(gPIN, LOW);
    digitalWrite(rPIN, LOW);
}

void uptimeBlynk() {
    static ulong lastTime = 0;
    if (!IsReady(lastTime, 1000)) return; // Kiểm tra và cập nhật mỗi giây
    ulong value = millis() / 1000;
    Blynk.virtualWrite(V0, value);
}

BLYNK_WRITE(V3) {
    buttonState = param.asInt();
}

void loop() {
  if (!WelcomeDisplayTimeout()) return;
  ThreeLedBlink();
  updateDisplay(); 
  uptimeBlynk();
  if (buttonState) {
      BlinkingYellowLight();
  }
}

