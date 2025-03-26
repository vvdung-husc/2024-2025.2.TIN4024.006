#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* BOT_TOKEN = "7466092720:AAHqng2K6fEUzhlzrncJFkhYws6yCI75rus"; // Thay bằng Token API của bot
const String CHAT_ID = "2017114551"; // Thay bằng Chat ID của bạn

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define PIR_PIN 23 // Chân kết nối cảm biến PIR
int pirState = LOW;

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi đã kết nối!");

    client.setInsecure(); // Bỏ qua chứng chỉ SSL

    pinMode(PIR_PIN, INPUT);
}

void loop() {
    int motion = digitalRead(PIR_PIN);
    
    if (motion == HIGH && pirState == LOW) {
        Serial.println("Chuyển động được phát hiện!");
        bot.sendMessage(CHAT_ID, "⚠️ Motion detected!", "");
        pirState = HIGH;
    } else if (motion == LOW && pirState == HIGH) {
        pirState = LOW;
    }

    delay(500);
}
