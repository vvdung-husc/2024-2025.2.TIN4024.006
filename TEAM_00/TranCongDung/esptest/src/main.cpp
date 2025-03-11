#define BLYNK_TEMPLATE_ID "TMPL6SS1f0G7n"
#define BLYNK_TEMPLATE_NAME "tcd"
#define BLYNK_AUTH_TOKEN "u1Gt11heKkrE9p1mC7KyLJmxOVg4t9E6"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


// WiFi Credentials
char ssid[] = "Wokwi-GUEST";  
char pass[] = ""; 

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cảm biến DHT22
#define DHTPIN 26
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Cảm biến khói
#define SMOKE_SENSOR_PIN 34  
#define SMOKE_THRESHOLD 800  
#define TEMP_THRESHOLD 30    

// Relay
#define RELAY_PIN 12

// LED
#define LED_RED 14
#define LED_GREEN 27

// Button
#define BUTTON_PIN 25

bool systemOn = true;
bool lastButtonState = HIGH;
bool relayState = LOW; // Trạng thái relay
unsigned long lastDHTRead = 0;
unsigned long lastSmokeRead = 0;
unsigned long lastButtonCheck = 0;
const long DHT_INTERVAL = 2000;
const long SMOKE_INTERVAL = 1000;
const long BUTTON_DEBOUNCE = 300;

void checkButton(unsigned long currentMillis);
void readDHTSensor(unsigned long currentMillis);
void readSmokeSensor(unsigned long currentMillis, float temperature, float humidity);

void setup() {
    Serial.begin(115200);

    // Khởi động WiFi & Blynk
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, relayState);

    dht.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        while (1);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.display();
}

void loop() {
    unsigned long currentMillis = millis();
    checkButton(currentMillis);

    if (systemOn) {
        readDHTSensor(currentMillis);
    } else {
        digitalWrite(LED_RED, LOW);
        digitalWrite(RELAY_PIN, LOW);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(10, 20);
        display.println("OFF");
        display.display();
    }
    Blynk.run();
}

// Nút nhấn bật/tắt hệ thống
void checkButton(unsigned long currentMillis) {
    bool buttonState = digitalRead(BUTTON_PIN);

    if (currentMillis - lastButtonCheck >= BUTTON_DEBOUNCE) {
        lastButtonCheck = currentMillis;

        if (buttonState == LOW && lastButtonState == HIGH) {  // Phát hiện nhấn nút
            systemOn = !systemOn;  // Đảo trạng thái hệ thống

            relayState = systemOn ? HIGH : LOW;  
            digitalWrite(RELAY_PIN, relayState);
            digitalWrite(LED_GREEN, systemOn);
            Blynk.virtualWrite(V0, relayState);

            Serial.printf("🔘 Button Pressed -> System: %s, Relay: %s\n",
                          systemOn ? "ON" : "OFF",
                          relayState ? "ON" : "OFF");
        }
        lastButtonState = buttonState;
    }
}

// Đọc cảm biến DHT22
// void readDHTSensor(unsigned long currentMillis) {
//     if (currentMillis - lastDHTRead >= DHT_INTERVAL) {
//         lastDHTRead = currentMillis;
//         float temperature = dht.readTemperature();
//         float humidity = dht.readHumidity();

//         if (!isnan(temperature) && !isnan(humidity)) {
//             Serial.printf("Temp: %.1f°C, Hum: %.1f%%\n", temperature, humidity);
            
//             Blynk.virtualWrite(V1, humidity);
//             Blynk.virtualWrite(V2, temperature);

//             readSmokeSensor(currentMillis, temperature, humidity);
//         } else {
//             Serial.println("⚠️ Error reading DHT sensor!");
//         }
//     }
// }

void readDHTSensor(unsigned long currentMillis) {
    if (currentMillis - lastDHTRead >= DHT_INTERVAL) {
        lastDHTRead = currentMillis;
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (!isnan(temperature) && !isnan(humidity)) {
            static float lastTemp = 0, lastHum = 0;
            
            if (abs(temperature - lastTemp) >= 0.5) {  // Chỉ gửi khi thay đổi ≥ 0.5°C
                Blynk.virtualWrite(V2, temperature);
                lastTemp = temperature;
            }

            if (abs(humidity - lastHum) >= 2) {  // Chỉ gửi khi thay đổi ≥ 2%
                Blynk.virtualWrite(V1, humidity);
                lastHum = humidity;
            }

            readSmokeSensor(currentMillis, temperature, humidity);
        } else {
            Serial.println("⚠️ Lỗi đọc DHT22!");
        }
    }
}


// Đọc cảm biến khói
void readSmokeSensor(unsigned long currentMillis, float temperature, float humidity) {
    if (currentMillis - lastSmokeRead >= SMOKE_INTERVAL) {
        lastSmokeRead = currentMillis;
        int smokeValue = random(300, 1200);  // Giả lập khói trong phạm vi hợp lý

        Serial.printf("Smoke Level: %d\n", smokeValue);

        Blynk.virtualWrite(V3, smokeValue);

        display.clearDisplay();
        display.setTextSize(2);

        if (smokeValue > SMOKE_THRESHOLD && temperature > TEMP_THRESHOLD) {
            Serial.println("🚨 DANGER: Smoke and High Temperature!");
            digitalWrite(LED_RED, HIGH);
            digitalWrite(RELAY_PIN, HIGH);
            relayState = HIGH;
            Blynk.virtualWrite(V0, relayState);
            display.setCursor(10, 20);
            display.println("DANGER!");
        } else if (smokeValue > SMOKE_THRESHOLD) {
            Serial.println("⚠️ WARNING: Smoke Detected!");
            digitalWrite(LED_RED, HIGH);
            display.setCursor(10, 20);
            display.println("SMOKE!");
        } else {
            Serial.println("✅ Air is Clear.");
            digitalWrite(LED_RED, temperature > TEMP_THRESHOLD ? HIGH : LOW);
            display.setCursor(10, 5);
            display.printf("T:%.1fC", temperature);
            display.setCursor(10, 30);
            display.printf("H:%.1f%%", humidity);
        }
        display.display();
    }
}

// void readSmokeSensor(unsigned long currentMillis, float temperature, float humidity) {
//     if (currentMillis - lastSmokeRead >= SMOKE_INTERVAL) {
//         lastSmokeRead = currentMillis;
//         int smokeValue = analogRead(SMOKE_SENSOR_PIN);

//         static int lastSmoke = 0;
//         if (abs(smokeValue - lastSmoke) >= 50) {  // Chỉ gửi khi thay đổi ≥ 50
//             Blynk.virtualWrite(V3, smokeValue);
//             lastSmoke = smokeValue;
//         }
//     }
// }


// Điều khiển relay từ Blynk
BLYNK_WRITE(V0) {
    relayState = param.asInt();  // Lấy giá trị từ Blynk
    digitalWrite(RELAY_PIN, relayState);
    systemOn = relayState;  // Đồng bộ trạng thái hệ thống với Blynk

   // Serial.printf("🌐 Blynk -> Relay State: %s\n", relayState ? "ON" : "OFF");
}


// Đồng bộ trạng thái relay khi Blynk kết nối
BLYNK_CONNECTED() {
    Blynk.syncVirtual(V0);
}
