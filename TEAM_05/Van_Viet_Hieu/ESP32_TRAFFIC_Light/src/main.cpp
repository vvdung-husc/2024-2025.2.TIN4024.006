#include <Arduino.h>
#include <TM1637Display.h>

// Pin định nghĩa
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

// Định nghĩa thời gian đèn sáng (ms)
#define rTIME  5000   // 5 giây
#define yTIME  2000   // 2 giây
#define gTIME  7000   // 7 giây

ulong previousMillis = 0;
ulong interval = rTIME;
int currentLED = rLED;

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  display.setBrightness(7);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  
  Serial.println("== START ==> Traffic Light");
}

void countdownDisplay(int timeLeft) {
  display.showNumberDec(timeLeft, true, 2, 2);
}

void loop() {
  ulong currentMillis = millis();
  int timeLeft = (interval - (currentMillis - previousMillis)) / 1000;

  // Chỉ cập nhật hiển thị khi giá trị thay đổi
  static int lastTimeLeft = -1;
  if (timeLeft != lastTimeLeft) {
      display.showNumberDec(timeLeft, true, 2, 2);
      lastTimeLeft = timeLeft;
  }

  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      switch (currentLED) {
          case rLED:
              digitalWrite(rLED, LOW);
              digitalWrite(gLED, HIGH);
              currentLED = gLED;
              interval = gTIME;
              break;
          case gLED:
              digitalWrite(gLED, LOW);
              digitalWrite(yLED, HIGH);
              currentLED = yLED;
              interval = yTIME;
              break;
          case yLED:
              digitalWrite(yLED, LOW);
              digitalWrite(rLED, HIGH);
              currentLED = rLED;
              interval = rTIME;
              break;
      }
  }
}
