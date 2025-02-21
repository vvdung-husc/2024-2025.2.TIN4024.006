#include <Arduino.h>
#include <TM1637Display.h>

// Pin
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

// Thời gian (ms)
#define rTIME  5000   // Đèn đỏ 5 giây
#define yTIME  2000   // Đèn vàng 2 giây
#define gTIME  7000   // Đèn xanh 7 giây
//a
ulong ledTimeStart = 0;
int currentLED = rLED;
int countdown = rTIME / 1000;

TM1637Display display(CLK, DIO);

void updateDisplay(int timeLeft) {
    display.showNumberDec(timeLeft, true, 2, 2);
}

void setup() {
    Serial.begin(115200);
    pinMode(rLED, OUTPUT);
    pinMode(yLED, OUTPUT);
    pinMode(gLED, OUTPUT);
    
    display.setBrightness(7);
    
    digitalWrite(yLED, LOW);
    digitalWrite(gLED, LOW);
    digitalWrite(rLED, HIGH);
    
    currentLED = rLED;
    ledTimeStart = millis();
    countdown = rTIME / 1000;
    updateDisplay(countdown);
    Serial.println("== START ==> RED");
}

void loop() {
    ulong currentMillis = millis();
    
    if (currentMillis - ledTimeStart >= 1000) {
        ledTimeStart = currentMillis;
        countdown--;
        updateDisplay(countdown);
        
        if (countdown == 0) {
            switch (currentLED) {
                case rLED:
                    digitalWrite(rLED, LOW);
                    digitalWrite(gLED, HIGH);
                    currentLED = gLED;
                    countdown = gTIME / 1000;
                    Serial.println("GREEN");
                    break;
                case gLED:
                    digitalWrite(gLED, LOW);
                    digitalWrite(yLED, HIGH);
                    currentLED = yLED;
                    countdown = yTIME / 1000;
                    Serial.println("YELLOW");
                    break;
                case yLED:
                    digitalWrite(yLED, LOW);
                    digitalWrite(rLED, HIGH);
                    currentLED = rLED;
                    countdown = rTIME / 1000;
                    Serial.println("RED");
                    break;
            }
            updateDisplay(countdown);
        }
    }
}
