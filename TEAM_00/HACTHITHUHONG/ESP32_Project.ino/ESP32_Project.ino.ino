#include <Arduino.h>
#include <U8g2lib.h>

// Định nghĩa chân GPIO cho đèn LED
#define GREEN_LED 15
#define YELLOW_LED 2
#define RED_LED 5
#define TEMP_SENSOR_PIN 16

// Khai báo màn hình OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 13, /* reset=*/ U8X8_PIN_NONE);

void setup() {
    Serial.begin(115200);

    // Cấu hình LED là OUTPUT
    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    
    // Khởi tạo màn hình OLED
    u8g2.begin();
}

void loop() {
    // Nhấp nháy LED
    digitalWrite(GREEN_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);

    digitalWrite(YELLOW_LED, HIGH);
    delay(500);
    digitalWrite(YELLOW_LED, LOW);

    digitalWrite(RED_LED, HIGH);
    delay(500);
    digitalWrite(RED_LED, LOW);

    // Đọc giá trị nhiệt độ (giả lập, vì ESP32 không có cảm biến nhiệt độ tích hợp)
    int temp_value = analogRead(TEMP_SENSOR_PIN);
    float temperature = (temp_value / 4095.0) * 100.0;

    // Hiển thị lên OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "ESP32 Project");
    
    char temp_str[20];
    sprintf(temp_str, "Temp: %.2f C", temperature);
    u8g2.drawStr(0, 30, temp_str);

    u8g2.sendBuffer();

    delay(1000);
}
