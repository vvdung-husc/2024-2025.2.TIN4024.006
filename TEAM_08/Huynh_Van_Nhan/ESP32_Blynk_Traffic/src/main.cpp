//Huỳnh Văn Nhân
#define BLYNK_TEMPLATE_ID "TMPL6PqjG-L40"
#define BLYNK_TEMPLATE_NAME "Nhan"
#define BLYNK_AUTH_TOKEN "IOg3DFoDvjBPSbxdbu3OOYGlKXGeGv52"


//Nguyễn Khánh Phượng
//#define BLYNK_TEMPLATE_ID "TMPL6Ovhz1RbQ"
//#define BLYNK_TEMPLATE_NAME "ESP32 Traffic Light Blynk"
//#define BLYNK_AUTH_TOKEN "LDnnRXGZFlpczlq4J5GHdgLHwA86hqK0"


#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// Thư viện cần thiết
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TM1637Display.h>

#define DHTPIN 16
#define DHTTYPE DHT22
#define GREEN_LED 25
#define YELLOW_LED 26
#define RED_LED 27
#define BLUE_LED 21
#define LDR_PIN 34
#define BUTTON_PIN 23
#define CLK 18
#define DIO 19

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

bool ledState = false;
bool displayState = false;
unsigned long lastButtonPress = 0;
float temperature = 0;
float humidity = 0;
bool useBlynkData = false;

int counter = 0;
int lightState = 0;
unsigned long lightTimer = 0;
int countdown = 5;
bool useBlynkLightData = false;

int darkThreshold = 1000;
bool lowLightMode = false;

float lastTemperature = -1;
float lastHumidity = -1;
int lastLightLevel = -1;
int lastLightState = -1;

// Kết nối WiFi
void connectWiFi() {
    Serial.print("Đang kết nối WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Đã kết nối!");
}

// Gửi dữ liệu cảm biến lên Blynk
void sendSensorData() {
    if (!useBlynkData) {
        float newTemp = dht.readTemperature();
        float newHumidity = dht.readHumidity();
        if (!isnan(newTemp) && !isnan(newHumidity)) {
            temperature = newTemp;
            humidity = newHumidity;
        }
    }

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Lỗi đọc cảm biến!");
        return;
    }

    
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V3, counter);
    counter++;

    if (temperature != lastTemperature || humidity != lastHumidity) {
        Serial.print("Nhiệt độ: ");
        Serial.print(temperature);
        Serial.print(" °C, Độ ẩm: ");
        Serial.print(humidity);
        Serial.println(" %");
        lastTemperature = temperature;
        lastHumidity = humidity;
    }
}

// Nhận giá trị ngưỡng ánh sáng từ Blynk
BLYNK_WRITE(V6) {
    darkThreshold = param.asInt();
    useBlynkLightData = true;
    Serial.print("Cập nhật ngưỡng ánh sáng từ Blynk: ");
    Serial.println(darkThreshold);
}

// Nhận nhiệt độ từ Blynk
BLYNK_WRITE(V4) {
    temperature = param.asFloat();
    useBlynkData = true;
}

// Nhận độ ẩm từ Blynk
BLYNK_WRITE(V5) {
    humidity = param.asFloat();
    useBlynkData = true;
}

// Kiểm tra nút nhấn và cập nhật trạng thái màn hình
void checkButton() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        if (millis() - lastButtonPress > 300) {
            ledState = !ledState;
            digitalWrite(BLUE_LED, ledState);
            Blynk.virtualWrite(V2, ledState);
            displayState = !displayState;
            if (!displayState) {
                display.clear();
            } else {
                display.showNumberDec(countdown);
            }

            lastButtonPress = millis();
        }
    }
}

// Điều khiển LED từ Blynk
BLYNK_WRITE(V2) {
    ledState = param.asInt();
    digitalWrite(BLUE_LED, ledState);

    displayState = ledState;

    if (!displayState) {
        display.clear();
    } else {
        display.showNumberDec(countdown);
    }
}

// Quản lý ánh sáng thấp

void checkLightLevel() {
    int lightLevel = analogRead(LDR_PIN);
    if (useBlynkLightData) {
        // Ưu tiên sử dụng ngưỡng từ Blynk để quyết định lowLightMode
        lowLightMode = (darkThreshold < 1000);
    } else {
        // Nếu chưa có dữ liệu từ Blynk, tự động lấy từ cảm biến
        darkThreshold = lightLevel;
        lowLightMode = (darkThreshold < 1000);
    }

    if (lightLevel != lastLightLevel) {
        Serial.print("Ánh sáng hiện tại: ");
        Serial.println(lightLevel);
        lastLightLevel = lightLevel;
    }
}

void updateTrafficLights() {
    checkLightLevel();

    if (lowLightMode) {
        // Chế độ ánh sáng yếu: Đèn vàng nhấp nháy, màn hình hiển thị "0000"
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, LOW);
        for (int i = 0; i < 5; i++) {
            digitalWrite(YELLOW_LED, HIGH);
            delay(500);
            digitalWrite(YELLOW_LED, LOW);
            delay(500);
        }

        if (displayState) {
            display.showNumberDec(0000);
        }
    } else {
        // Chế độ giao thông bình thường
        if (millis() - lightTimer >= 1000) {
            lightTimer = millis();

            if (countdown > 0) {
                countdown--;
            } else {
                lightState = (lightState + 1) % 3;
                countdown = (lightState == 0) ? 5 : (lightState == 1) ? 2 : 3;
            }
        }

        digitalWrite(GREEN_LED, lightState == 0);
        digitalWrite(YELLOW_LED, lightState == 1);
        digitalWrite(RED_LED, lightState == 2);

        // Cập nhật màn hình LED nếu đang bật
        if (displayState) {
            display.showNumberDec(countdown);
        }

        if (lightState != lastLightState) {
            Serial.println(lightState == 0 ? "🟢 Xanh" : lightState == 1 ? "🟡 Vàng" : "🔴 Đỏ");
            lastLightState = lightState;
        }
    }
}

// Cấu hình hệ thống
void setup() {
    Serial.begin(115200);
    connectWiFi();
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);

    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
    display.clear();
    Blynk.virtualWrite(V3, counter);
    Blynk.virtualWrite(V6, darkThreshold);
    dht.begin();
    display.setBrightness(7);
    timer.setInterval(2000L, sendSensorData);
}

void loop() {
    Blynk.run();
    timer.run();
    checkButton();
    updateTrafficLights();
}
