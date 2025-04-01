#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>

#define BLYNK_TEMPLATE_ID "TMPL6LsgAX3Th"
#define BLYNK_TEMPLATE_NAME "ESP8266 Project Blynk"
#define BLYNK_AUTH_TOKEN "NRxDrvNE-3IC_sAFQrndANvDQBM1Sl9P"


#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "utils.h"

// Thông tin WiFi và Blynk
char auth[] = "NRxDrvNE-3IC_sAFQrndANvDQBM1Sl9P";
char ssid[] = "CNTT-MMT";
char pass[] = "13572468";

// Định nghĩa chân LED và nút nhấn
#define LED_RED    5
#define LED_YELLOW 2
#define LED_GREEN  15
#define BTN_MODE   16

// Khai báo màn hình OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* SCL=*/ 12, /* SDA=*/ 13);

// Biến lưu trữ
unsigned long timerTemp = 0;
unsigned long timerUptime = 0;
bool yellowBlinkMode = false;
unsigned long startTime;

// 📌 Gửi thời gian hoạt động lên Blynk (V5)
void sendUptime() {
    unsigned long uptime = (millis() - startTime) / 1000; // Chuyển millis thành giây
    Blynk.virtualWrite(V0, uptime); // Gửi dữ liệu lên Label V5
}
void handleTemperatureHumidity();
void handleTrafficLight();
void handleUptime();
void sendUptime();

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);
    Blynk.begin(auth, ssid, pass);

    // Khởi động màn hình OLED
    u8g2.begin();
    
    // Cấu hình chân LED và nút nhấn
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(BTN_MODE, INPUT_PULLUP);

    startTime = millis();
}

void loop() {
    Blynk.run();
    handleTemperatureHumidity();
    handleTrafficLight();
    handleUptime();
}

// 📌 Sinh số ngẫu nhiên và hiển thị lên OLED
void handleTemperatureHumidity() {
    if (!IsReady(timerTemp, 2000)) return;

    float temperature = random(-40, 81);
    float humidity = random(0, 101);

    Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 20, StringFormat("Temp: %.1fC", temperature).c_str());
    u8g2.drawStr(0, 40, StringFormat("Humidity: %.1f%%", humidity).c_str());
    u8g2.sendBuffer();

    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
}

// 📌 Điều khiển đèn giao thông
void handleTrafficLight() {
    static unsigned long lastBlink = 0;
    static bool lastButtonState = HIGH;
    
    bool buttonState = digitalRead(BTN_MODE);
    
    // Kiểm tra nếu nút được nhấn (chống dội)
    if (buttonState == LOW && lastButtonState == HIGH) {
        yellowBlinkMode = !yellowBlinkMode;
        Blynk.virtualWrite(V3, yellowBlinkMode);
        delay(300);
    }
    lastButtonState = buttonState;

    if (yellowBlinkMode) {
        if (millis() - lastBlink > 500) {
            digitalWrite(LED_YELLOW, !digitalRead(LED_YELLOW));
            lastBlink = millis();
        }
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
    } else {
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
        delay(3000);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        delay(3000);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        delay(1000);
        digitalWrite(LED_YELLOW, LOW);
    }
}

// 📌 Gửi thời gian hoạt động mỗi giây
void handleUptime() {
    if (!IsReady(timerUptime, 1000)) return;
    sendUptime();
}

// 📌 Nhận tín hiệu từ Blynk để điều khiển chế độ đèn vàng nhấp nháy
BLYNK_WRITE(V3) {
    yellowBlinkMode = param.asInt();
}
