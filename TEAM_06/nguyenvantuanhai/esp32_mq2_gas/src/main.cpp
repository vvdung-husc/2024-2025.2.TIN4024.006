
// Blynk
#define BLYNK_TEMPLATE_ID "TMPL68YCNBftz"
#define BLYNK_TEMPLATE_NAME "Esp32 Mq2 Gas Sensor"
#define BLYNK_AUTH_TOKEN "BIL_HGBSoFrOv7uFukwQV7FxIAkPCnHl"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Telegram
#define BOT_TOKEN "7462780055:AAESMhTWsftegxA7D_k1kMKkwx9smS9lHJs"
#define CHAT_ID "7413444616"

// Pin setup
#define GAS_AOUT_PIN 34  // analog MQ-2
#define GAS_DOUT_PIN 25  // digital MQ-2 (không dùng trong bản này)
#define LED_RED      22  // LED cảnh báo vật lý
#define BUZZER_PIN   26  // Còi báo động

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastAlertTime = 0;
int dangerThreshold = 3000;  // Ngưỡng khí gas nguy hiểm

void setup() {
  Serial.begin(115200);

  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  client.setInsecure();  // Bỏ kiểm tra SSL

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();

  int gasLevel = analogRead(GAS_AOUT_PIN);
  Serial.print("Gas Level: ");
  Serial.println(gasLevel);

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V1, gasLevel);  // Nồng độ khí gas

  if (gasLevel >= dangerThreshold) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    Blynk.virtualWrite(V2, "⚠️ Nguy hiểm! Phát hiện rò rỉ khí gas");
    Blynk.virtualWrite(V3, 255);  // LED cảnh báo Blynk sáng

    if (millis() - lastAlertTime > 30000) {
      String msg = "⚠️ CẢNH BÁO: Phát hiện rò rỉ khí gas!\nNồng độ: " + String(gasLevel);
      bot.sendMessage(CHAT_ID, msg, "");
      lastAlertTime = millis();
    }
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    Blynk.virtualWrite(V2, "✅ An toàn");
    Blynk.virtualWrite(V3, 0);  // LED cảnh báo tắt
  }

  delay(1000);
}
