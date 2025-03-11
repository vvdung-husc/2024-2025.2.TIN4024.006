#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <TM1637Display.h>
#include <DHT.h>

// Thông tin Blynk
#define BLYNK_TEMPLATE_ID "TMPL6Y678JdmS"
#define BLYNK_TEMPLATE_NAME "ESP32 BLYNK with TEMPERATURE"
#define BLYNK_AUTH_TOKEN "2j6xfI8bmAVh4dHaPkp0wvrNJ8nYQ5bo"
#include <BlynkSimpleEsp32.h>


#define BLYNK_PRINT Serial

// WiFi
char ssid[] = "Wokwi-GUEST";  
char pass[] = "";

// Định nghĩa chân
#define LED_PIN 21
#define BUTTON_PIN 23
#define DHT_PIN 16
#define DHT_TYPE DHT22
#define CLK 18
#define DIO 19

// Chân Blynk
#define BLYNK_LED V1
#define BLYNK_TEMP V2
#define BLYNK_HUMI V3
#define BLYNK_TM1637 V4
#define BLYNK_UPTIME V5  
#define BLYNK_STATUS V6  // Trạng thái bộ đếm

// Khai báo đối tượng
DHT dht(DHT_PIN, DHT_TYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;

bool counterRunning = false; // Trạng thái bộ đếm
int counter = 0;
long uptime = 0;

// Hàm cập nhật thời gian hoạt động
void sendUptime() {
    uptime++;  
    Blynk.virtualWrite(BLYNK_UPTIME, uptime);  
    display.showNumberDec(uptime % 10000);  
}

// Khi thay đổi trạng thái LED trên Blynk
BLYNK_WRITE(BLYNK_LED) {
    int p = param.asInt();
    digitalWrite(LED_PIN, p);
}

// Khi nhận dữ liệu từ Blynk để điều khiển TM1637
BLYNK_WRITE(BLYNK_TM1637) {
    int value = param.asInt();
    display.showNumberDec(value);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    display.setBrightness(7);
    display.showNumberDec(0);
    dht.begin();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    
    timer.setInterval(1000L, sendUptime);
}

void loop() {
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    Blynk.virtualWrite(BLYNK_TEMP, temp);
    Blynk.virtualWrite(BLYNK_HUMI, humidity);

    // Kiểm tra nút nhấn
    if (digitalRead(BUTTON_PIN) == LOW) {
        delay(200); // Chống dội phím

        unsigned long pressTime = millis(); // Lưu thời điểm nhấn nút

        // Chờ nút nhấn giữ trong 2 giây để reset
        while (digitalRead(BUTTON_PIN) == LOW) {
            if (millis() - pressTime > 2000) { // Nếu nhấn quá 2 giây
                counter = 0;  // Reset bộ đếm
                counterRunning = false;  // Tắt bộ đếm
                digitalWrite(LED_PIN, LOW);
                display.showNumberDec(0);  // Hiển thị 0 trên TM1637
                Blynk.virtualWrite(BLYNK_TM1637, 0); // Gửi giá trị 0 lên Blynk
                Blynk.virtualWrite(BLYNK_STATUS, "OFF"); // Cập nhật trạng thái OFF lên Blynk
                break;
            }
        }

        // Nếu nhấn ngắn hơn 2 giây, đổi trạng thái bộ đếm
        if (millis() - pressTime < 2000) {
            counterRunning = !counterRunning;  // Đảo trạng thái bộ đếm
            Blynk.virtualWrite(BLYNK_STATUS, counterRunning ? "ON" : "OFF"); // Gửi trạng thái lên Blynk
        }

        delay(200); // Chống dội phím
    }

    // Nếu bộ đếm đang chạy, tăng giá trị
    if (counterRunning) {
        counter++;
        display.showNumberDec(counter);
        Blynk.virtualWrite(BLYNK_TM1637, counter);
        delay(1000);  // Chạy bộ đếm mỗi giây
    }

    Blynk.run();
    timer.run();  
}
