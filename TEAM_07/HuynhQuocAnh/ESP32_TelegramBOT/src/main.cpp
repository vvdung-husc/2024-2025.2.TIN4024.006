#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Thông tin WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Thông tin Bot Telegram
const char* BOT_TOKEN = "7466092720:AAHqng2K6fEUzhlzrncJFkhYws6yCI75rus"; // Thay bằng Token API của bot
const String CHAT_ID = "-1002638275054"; // ID nhóm

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

#define PIR_PIN 23 // Chân cảm biến PIR
int pirState = LOW;

void setup() {
    Serial.begin(115200);

    // Kết nối WiFi
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi đã kết nối!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    client.setInsecure(); // Bỏ qua chứng chỉ SSL (phải làm sau khi WiFi đã kết nối)

    pinMode(PIR_PIN, INPUT);
}

void loop() {
    int motion = digitalRead(PIR_PIN);
    
    if (motion == HIGH && pirState == LOW) {
        Serial.println("Chuyển động được phát hiện!");
        
        // Gửi tin nhắn và kiểm tra lỗi
        bool success = bot.sendMessage(CHAT_ID, "⚠️ Motion detected!", "");
        if (success) {
            Serial.println("Gửi tin nhắn thành công!");
        } else {
            Serial.println("Gửi tin nhắn thất bại!");
        }

        pirState = HIGH;
    } else if (motion == LOW && pirState == HIGH) {
        pirState = LOW;
    }

    delay(500);
}
