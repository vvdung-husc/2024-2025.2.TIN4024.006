#include <Arduino.h>
#include <U8g2lib.h>

#define GREEN 15
#define YELLOW 2
#define RED 5
#define TEMP_SENSOR_PIN 16  // Cảm biến nhiệt độ

// Khởi tạo màn hình OLED SH1106
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* SCL=*/ 12, /* SDA=*/ 13);

void setup() {
    pinMode(GREEN, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT);

    u8g2.begin();
}

void loop() {
    // Đọc giá trị cảm biến nhiệt độ (giả sử tín hiệu analog)
    int sensorValue = analogRead(TEMP_SENSOR_PIN);
    float temperature = sensorValue * (3.3 / 10240.0) * 100.0; // Quy đổi giá trị

    // Hiển thị lên màn hình OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 20);
    u8g2.print("Temperature:  ");
    u8g2.print(temperature, 1);
    u8g2.print(" *C");
    u8g2.sendBuffer();

    // Nhấp nháy đèn
    digitalWrite(GREEN, HIGH);
    delay(500);
    digitalWrite(GREEN, LOW);
    
    digitalWrite(YELLOW, HIGH);
    delay(500);
    digitalWrite(YELLOW, LOW);
    
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
}