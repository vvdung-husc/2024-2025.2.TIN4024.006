#include <Arduino.h>
#include <TM1637Display.h>

// Khai báo chân kết nối
#define GREEN_LED 25
#define YELLOW_LED 26
#define RED_LED 27
#define BLUE_LED 21 // Đèn xanh dương
#define BUTTON 23 // Nút nhấn xanh dương
#define LDR_PIN 13 // Cảm biến ánh sáng
#define CLK 18
#define DIO 19

TM1637Display display(CLK, DIO);
bool displayEnabled = true;
bool nightMode = false;
int state = 0;
unsigned long lastChange = 0;
const int durations[] = {6, 2, 3}; // Thời gian tương ứng cho đèn xanh lá, vàng, đỏ
const char* colorNames[] = {"Gr", "Ye", "Re"};

void setup() {
    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);
    display.setBrightness(7);
    
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
}

void loop() {
    static int remainingTime = durations[state];
    static bool buttonPressed = false;
    static unsigned long lastBlink = 0;
    static bool yellowState = false;
    
    int lightLevel = analogRead(LDR_PIN);
    nightMode = (lightLevel < 1000); // Ngưỡng ánh sáng để kích hoạt chế độ đêm
    
    if (nightMode) {
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        if (millis() - lastBlink >= 500) { // Nhấp nháy đèn vàng
            lastBlink = millis();
            yellowState = !yellowState;
            digitalWrite(YELLOW_LED, yellowState);
        }
        display.showNumberDec(0, true); // Hiển thị 0 khi trời tối
        return;
    }

    if (digitalRead(BUTTON) == LOW && !buttonPressed) {
        displayEnabled = !displayEnabled;
        digitalWrite(BLUE_LED, displayEnabled ? HIGH : LOW);
        buttonPressed = true;
    }
    if (digitalRead(BUTTON) == HIGH) {
        buttonPressed = false;
    }

    if (millis() - lastChange >= 1000) {
        lastChange = millis();
        remainingTime--;
        if (displayEnabled) {
            display.showNumberDec(remainingTime, true);
        } else {
            display.clear();
        }
        
        if (remainingTime <= 0) {
            int prevState = state;
            state = (state + 1) % 3;
            remainingTime = durations[state];
            
            digitalWrite(GREEN_LED, state == 0);
            digitalWrite(YELLOW_LED, state == 1);
            digitalWrite(RED_LED, state == 2);
            
            // Hiển thị trạng thái chuyển đổi
            if (displayEnabled) {
                display.showNumberDecEx(durations[prevState] * 100 + durations[state], 0b01000000, true);
                delay(1000);
            }
        }
    }
}
