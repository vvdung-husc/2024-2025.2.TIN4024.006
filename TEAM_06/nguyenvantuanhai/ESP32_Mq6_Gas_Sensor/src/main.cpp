
#define BLYNK_TEMPLATE_ID "TMPL6ZyYBcFq2"
#define BLYNK_TEMPLATE_NAME "Esp32 Mq6 Gas Sensor"
#define BLYNK_AUTH_TOKEN "B_-hkxgqrtBwnl0speBNpAmL_JfFvLbM"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#define GAS_SENSOR_PIN 34   // Chân kết nối cảm biến MQ-6
#define BUZZER_PIN 25       // Chân kết nối còi báo động
#define GREEN_LED_PIN 18    // Đèn báo an toàn
#define YELLOW_LED_PIN 17   // Đèn cảnh báo nhẹ
#define RED_LED_PIN 19      // Đèn cảnh báo nguy hiểm

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

void connectWiFi() {
    Serial.print("Đang kết nối WiFi");
    WiFi.begin(ssid, pass);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        attempts++;
        if (attempts > 15) {
            Serial.println("\nKhông thể kết nối WiFi! Kiểm tra lại SSID/PASSWORD.");
            ESP.restart();
        }
    }
    Serial.println("\nĐã kết nối WiFi!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("[HỆ THỐNG PHÁT HIỆN RÒ RỈ KHÍ GAS]");
    Serial.println("[AN TOÀN] Ngưỡng an toàn: Dưới 200 ppm");
    Serial.println("[CẢNH BÁO NHẸ] Ngưỡng cảnh báo nhẹ: 200 - 1000 ppm");
    Serial.println("[NGUY HIỂM] Ngưỡng nguy hiểm: Trên 1000 ppm");
    
    pinMode(GAS_SENSOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    connectWiFi();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi mất kết nối! Đang kết nối lại...");
        connectWiFi();
    }
    
    Blynk.run();
    int gasValue = analogRead(GAS_SENSOR_PIN);
    Serial.print("[MỨC KHÍ GAS] ");
    Serial.print(gasValue);
    Serial.println(" ppm");
    
    Blynk.virtualWrite(V1, gasValue); // Gửi giá trị khí gas lên Blynk
    
    if (gasValue > 1000) {
        Serial.println("[!!! NGUY HIỂM !!!] Có nguy cơ cháy nổ hoặc ảnh hưởng đến sức khỏe!");
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
        Blynk.virtualWrite(V5, 1);
        Blynk.virtualWrite(V4, 0);
        Blynk.virtualWrite(V3, 0);
        Blynk.virtualWrite(V2, "NGUY HIỂM");
    } else if (gasValue > 200) {
        Serial.println("[CẢNH BÁO NHẸ] Cần thông gió hoặc kiểm tra rò rỉ khí gas.");
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        Blynk.virtualWrite(V5, 0);
        Blynk.virtualWrite(V4, 1);
        Blynk.virtualWrite(V3, 0);
        Blynk.virtualWrite(V2, "CẢNH BÁO NHẸ");
    } else {
        Serial.println("[AN TOÀN] Mức khí gas bình thường.");
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
        Blynk.virtualWrite(V5, 0);
        Blynk.virtualWrite(V4, 0);
        Blynk.virtualWrite(V3, 1);
        Blynk.virtualWrite(V2, "AN TOÀN");
    }
    
    delay(1000);
}
