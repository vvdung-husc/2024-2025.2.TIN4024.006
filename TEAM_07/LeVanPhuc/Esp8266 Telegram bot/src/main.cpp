// #include <ESP8266WiFi.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>

// // Thông tin Wi-Fi
// const char* ssid = "CNTT-MMT";
// const char* password = "13572468";

// // Token bot Telegram
// const char* BOT_TOKEN = "8090161780:AAGlAHqQrQkF6K9_SMv-kOFwCPlpqg1e94Q";

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Thông tin Wi-Fi
const char* ssid = "CNTT-MMT";
const char* password = "13572468";

// Token bot Telegram
const char* BOT_TOKEN = "8090161780:AAGlAHqQrQkF6K9_SMv-kOFwCPlpqg1e94Q";

// Kết nối Telegram
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Thiết lập màn hình OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Thời gian kiểm tra tin nhắn
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void setup() {
  Serial.begin(115200);
  
  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Đang kết nối Wi-Fi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi đã kết nối");
  client.setInsecure();

  // Khởi tạo OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Không tìm thấy màn hình OLED!");
    while (true);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Bot Telegram ON!");
  display.display();
}

void loop() {
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while (numNewMessages) {
      Serial.println("Nhận tin nhắn mới!");
      
      for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        Serial.println("Tin nhắn: " + text);

        // Hiển thị tin nhắn lên OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Tin nhắn:");
        display.setCursor(0, 20);
        display.println(text);
        display.display();

        // Xử lý lệnh từ Telegram
        if (text == "/start") {
          bot.sendMessage(chat_id, "Xin chào! Nhập lệnh /on hoặc /off.", "");
        } else if (text == "/on") {
          bot.sendMessage(chat_id, "Bật thiết bị!", "");
        } else if (text == "/off") {
          bot.sendMessage(chat_id, "Tắt thiết bị!", "");
        } else {
          bot.sendMessage(chat_id, "Lệnh không hợp lệ!", "");
        }
      }

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}
