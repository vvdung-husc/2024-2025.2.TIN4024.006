#include <Arduino.h>
#include <TM1637Display.h>

int led_green = 19;
int led_yellow = 17;
int led_red = 16;

unsigned long previousMillis = 0;
int state = 0;
int greenTime, redTime;
const int yellowTime = 3000;

// Cấu hình các chân của TM1637
TM1637Display display1(23, 18);  // TM1637 1 (DIO, CLK)
TM1637Display display2(4, 5);    // TM1637 2 (DIO, CLK)

void setup() {
    pinMode(led_green, OUTPUT);
    pinMode(led_yellow, OUTPUT);
    pinMode(led_red, OUTPUT);
    
    randomSeed(analogRead(0));
    redTime = random(1, 2)* 1000;
    greenTime = random(2, 3) * 1000;
    
    previousMillis = millis();
    
    // Khởi tạo hiển thị LED 7 đoạn
    display1.setBrightness(7); // Đặt độ sáng cho LED 7 đoạn
    display2.setBrightness(7);
}

void loop() {
    unsigned long currentMillis = millis();
    
    switch (state) {
        case 0: // Đèn đỏ
            digitalWrite(led_red, HIGH);
            digitalWrite(led_yellow, LOW);
            digitalWrite(led_green, LOW);
            display1.showNumberDec(redTime / 1000, false);  // Hiển thị thời gian đèn đỏ
            display2.showNumberDec(0, false); // Mặc định không hiển thị gì trên LED 2
            
            if (currentMillis - previousMillis >= redTime) {
                previousMillis = currentMillis;
                state = 1;
            }
            break;
        
        case 1: // Đèn xanh
            digitalWrite(led_red, LOW);
            digitalWrite(led_yellow, LOW);
            digitalWrite(led_green, HIGH);
            display1.showNumberDec(0, false);  // Mặc định không hiển thị gì trên LED 1
            display2.showNumberDec(greenTime / 1000, false);  // Hiển thị thời gian đèn xanh
            
            if (currentMillis - previousMillis >= greenTime) {
                previousMillis = currentMillis;
                state = 2;
            }
            break;
        
        case 2: // Đèn vàng
            digitalWrite(led_red, LOW);
            digitalWrite(led_yellow, HIGH);
            digitalWrite(led_green, LOW);
            display1.showNumberDec(yellowTime / 1000, false);  // Hiển thị thời gian đèn vàng
            display2.showNumberDec(0, false); // Mặc định không hiển thị gì trên LED 2
            
            if (currentMillis - previousMillis >= yellowTime) {
                previousMillis = currentMillis;
                state = 0;
                redTime = random(1, 2) * 1000;
                greenTime = random(2, 3) * 1000;
            }
            break;
    }
}
