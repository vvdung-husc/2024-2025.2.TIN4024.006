#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// WiFi Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Telegram Bot Token & Group ID
#define BOT_TOKEN "7996806265:AAHeTN2z6k0muLRw4X99sSDFv579rlnBcoU"
#define GROUP_ID "-4604565399"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

const int PIR_SENSOR_PIN = 27;
bool motionDetected = false;

// Motion detection ISR
void IRAM_ATTR onMotionDetected() {
    motionDetected = true;
}

void setup() {
    Serial.begin(115200);
    pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIR_SENSOR_PIN), onMotionDetected, RISING);

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nWiFi connected");
    bot.sendMessage(GROUP_ID, "ESP32 Telegram Bot Started");
}

void loop() {
    if (motionDetected) {
        Serial.println("Motion detected! Sending message to Telegram...");
        bot.sendMessage(GROUP_ID, "Hello world");
        Serial.println("Message sent successfully");
        motionDetected = false;
    }
}
