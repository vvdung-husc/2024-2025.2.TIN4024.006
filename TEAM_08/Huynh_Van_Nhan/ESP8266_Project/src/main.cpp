#define BLYNK_TEMPLATE_ID "TMPL6RQGDLOQe" 
#define BLYNK_TEMPLATE_NAME "ESP8266" 
#define BLYNK_AUTH_TOKEN "HkHpsRJ8SZ-wbZcBynAWmAqATjJOvHQV" 

#include <Arduino.h>
#include "utils.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include <U8g2lib.h>

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Thông tin WiFi
const char* ssid = "CNTT-MMT";    
const char* password = "13572468";

// Token bot Telegram
const char* botToken = "8027625067:AAHdREtzh1lGcXPLv2VNkIr8-Lo5ffGE3pI";

// Group ID Telegram
const String chatId = "-1002439452600";

// Chân LED điều khiển đèn giao thông
#define gPIN 15
#define yPIN 2
#define rPIN 5

#define OLED_SDA 13
#define OLED_SCL 12

// Khai báo đối tượng OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool blinkMode = false;
bool trafficActive = true;  // Biến để kiểm soát trạng thái đèn

unsigned long upTime = 0;
float fTemperature = 0.0;
float fHumidity = 0.0;

BlynkTimer timer;

// Khai báo đối tượng Telegram
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);
unsigned long lastTimeBotRan;
const int botRequestDelay = 1000;  // Kiểm tra tin nhắn mỗi giây

// Nhận lệnh từ Blynk để bật chế độ nhấp nháy đèn vàng
BLYNK_WRITE(V3) {
  blinkMode = param.asInt();
}

// Hiển thị trên OLED
void updateOLED() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese2);

  oled.drawUTF8(0, 14, ("Nhiet do: " + String(fTemperature, 1) + "°C").c_str());
  oled.drawUTF8(0, 28, ("Do am: " + String(fHumidity, 1) + "%").c_str());
  oled.drawUTF8(0, 42, ("Uptime: " + String(upTime) + "s").c_str());

  oled.sendBuffer();
}

// Điều khiển đèn giao thông
void TrafficLightControl() {
  static unsigned long lastTimer = 0;
  static int state = 0;
  static const unsigned long durations[] = {2000, 3000, 1000}; 
  static const int ledPins[] = {rPIN, gPIN, yPIN};

  if (!trafficActive) {  // Nếu đèn bị tắt từ Telegram
    digitalWrite(rPIN, LOW);
    digitalWrite(yPIN, LOW);
    digitalWrite(gPIN, LOW);
    return;
  }

  if (blinkMode) {  // Chế độ nhấp nháy đèn vàng
    if (millis() - lastTimer > 500) {
      lastTimer = millis();
      digitalWrite(yPIN, !digitalRead(yPIN));
    }
    digitalWrite(rPIN, LOW);
    digitalWrite(gPIN, LOW);
    return;
  }

  // Chế độ hoạt động bình thường
  if (millis() - lastTimer > durations[state]) {
    lastTimer = millis();
    digitalWrite(ledPins[state], LOW);
    state = (state + 1) % 3;
    digitalWrite(ledPins[state], HIGH);
  }
}

// Gửi dữ liệu lên Blynk
void sendToBlynk() {
  Blynk.virtualWrite(V0, upTime);      
  Blynk.virtualWrite(V1, fTemperature);
  Blynk.virtualWrite(V2, fHumidity);
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
}

// Cập nhật thời gian chạy
void updateupTime() {
  static unsigned long lastTimer = 0;
  if (millis() - lastTimer < 1000) return;
  lastTimer = millis();
  upTime++;
}

// Xử lý tin nhắn từ Telegram
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.println("Tin nhắn nhận được: " + text);

    if (chat_id == chatId) {  // Kiểm tra đúng group ID
      if (text == "/traffic_off") {
        trafficActive = false;
        digitalWrite(rPIN, LOW);
        digitalWrite(yPIN, LOW);
        digitalWrite(gPIN, LOW);
        bot.sendMessage(chatId, "🚦 Đèn giao thông đã tắt.", "");
      } else if (text == "/traffic_on") {
        trafficActive = true;
        bot.sendMessage(chatId, "🚦 Đèn giao thông hoạt động trở lại.", "");
      }
    }
  }
}

// Thêm biến thời gian cho cảnh báo Telegram
unsigned long lastAlertTime = 0;
const unsigned long alertInterval = 60 * 1000;  // 1 phút

// Gửi cảnh báo nếu nhiệt độ hoặc độ ẩm nguy hiểm
void checkAndSendAlert() {
  unsigned long currentTime = millis();
  if (currentTime - lastAlertTime < alertInterval) return; // Chỉ gửi mỗi 5 phút

  String alertMessage = "🚨 *CẢNH BÁO SỨC KHỎE!* 🚨\n";
  bool shouldSendAlert = false;

  if (fTemperature < 10) {
    alertMessage += "🌡️ *Nhiệt độ quá thấp!* Nguy cơ hạ thân nhiệt, tê cóng.\n";
    shouldSendAlert = true;
  } else if (fTemperature >= 30 && fTemperature <= 35) {
    alertMessage += "🔥 *Nhiệt độ cao!* Có thể gây mất nước, mệt mỏi.\n";
    shouldSendAlert = true;
  } else if (fTemperature > 35 && fTemperature <= 40) {
    alertMessage += "⚠️ *Cảnh báo nóng!* Nguy cơ sốc nhiệt, chuột rút, say nắng.\n";
    shouldSendAlert = true;
  } else if (fTemperature > 40) {
    alertMessage += "🚑 *Cực kỳ nguy hiểm!* Có thể gây suy nội tạng, đột quỵ nhiệt.\n";
    shouldSendAlert = true;
  }

  if (fHumidity < 30) {
    alertMessage += "💨 *Độ ẩm thấp!* Có thể gây khô da, kích ứng mắt.\n";
    shouldSendAlert = true;
  } else if (fHumidity > 70 && fHumidity <= 80) {
    alertMessage += "🌫️ *Độ ẩm cao!* Tăng nguy cơ vi khuẩn và nấm mốc.\n";
    shouldSendAlert = true;
  } else if (fHumidity > 80) {
    alertMessage += "🚨 *Độ ẩm quá cao!* Cơ thể khó thoát mồ hôi, dễ sốc nhiệt.\n";
    shouldSendAlert = true;
  }

  if (shouldSendAlert) {
    bot.sendMessage(chatId, alertMessage, "Markdown");
    lastAlertTime = currentTime; // Cập nhật thời gian gửi cảnh báo
  }
}



// SETUP
void setup() {
  Serial.begin(115200);

  // Cấu hình LED
  pinMode(gPIN, OUTPUT);
  pinMode(yPIN, OUTPUT);
  pinMode(rPIN, OUTPUT);
  digitalWrite(gPIN, LOW);
  digitalWrite(yPIN, LOW);
  digitalWrite(rPIN, HIGH);

  // Cấu hình OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_unifont_t_vietnamese1);
  oled.drawUTF8(0, 14, "Trường ĐHKH");
  oled.drawUTF8(0, 28, "Khoa CNTT");
  oled.drawUTF8(0, 42, "Lập trình IoT");
  oled.sendBuffer();

  // Kết nối WiFi & Blynk
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  client.setInsecure();  // Bỏ qua SSL
  bot.sendMessage(chatId, "🚦 Bot điều khiển đèn giao thông đã khởi động!", "");

  // Gửi dữ liệu lên Blynk mỗi 3 giây
  timer.setInterval(3000L, sendToBlynk);
}

// LOOP
void loop() {
  Blynk.run();
  timer.run();

  TrafficLightControl();
  updateSensorData();
  updateupTime();
  updateOLED();

  // Xử lý tin nhắn Telegram
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // Kiểm tra và gửi cảnh báo nếu cần
  checkAndSendAlert();
}
