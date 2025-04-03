#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Thông tin WiFi
const char* ssid = "FRAMING";
const char* password = "farming886";

// Token của bot Telegram
const char* botToken = "7901007042:AAGP1UE9XJbeCf4ZwEN9abqkV92H-4e-qLw";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    client.setInsecure(); // Bỏ qua kiểm tra chứng chỉ SSL
}

void loop() {
    int newMessages = bot.getUpdates(bot.last_message_received + 1);
    while (newMessages) {
        for (int i = 0; i < newMessages; i++) {
            String chat_id = bot.messages[i].chat_id;
            String text = bot.messages[i].text;
            Serial.println("Received: " + text);
            
            if (text == "/start") {
                bot.sendMessage(chat_id, "🤖 Xin chào! Đây là ESP8266 Telegram Bot.\nGõ /led_on để bật đèn hoặc /led_off để tắt đèn.", "Markdown");
            } else if (text == "/led_on") {
                digitalWrite(D4, HIGH);
                bot.sendMessage(chat_id, "💡 Đèn đã BẬT!", "Markdown");
            } else if (text == "/led_off") {
                digitalWrite(D4, LOW);
                bot.sendMessage(chat_id, "💡 Đèn đã TẮT!", "Markdown");
            } else {
                bot.sendMessage(chat_id, "Bạn đã gửi: " + text, "Markdown");
            }
        }
        newMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    delay(1000);
}

