#include <Arduino.h>
#include <TM1637Display.h>

// Pin
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

// Thời gian đèn (ms)
#define rTIME  5000   // 5 giây
#define yTIME  2000   // 2 giây
#define gTIME  7000   // 7 giây

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong displayTimeStart = 0;
int currentLED = rLED;
int counter = rTIME / 1000; // Bộ đếm ban đầu cho đèn đỏ

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();
void Testing_Display(int timeLeft);

void setup() {
  // Cài đặt ban đầu
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  display.setBrightness(7);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);

  Serial.println("== START ==> ");
  Serial.print("1. RED \t\t => Next "); Serial.println(rTIME);
}

void loop() {
  currentMiliseconds = millis();
  NonBlocking_Traffic_Light();

  // Hiển thị số giây còn lại
  int timeLeft = counter; // Truyền số giây còn lại vào Testing_Display
  Testing_Display(timeLeft);
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void NonBlocking_Traffic_Light() {
  switch (currentLED) {
    case rLED: // Đèn đỏ
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = gLED;
        counter = gTIME / 1000; // Reset bộ đếm theo thời gian đèn vàng
        Serial.print("2.YELLOW \t => Next "); Serial.println(gTIME);
      }
      break;

    case gLED: // Đèn vàng
      if (IsReady(ledTimeStart, gTIME)) {
        digitalWrite(yLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = yLED;
        counter = yTIME / 1000; // Reset bộ đếm theo thời gian đèn xanh
        Serial.print("3. GREEN\t => Next "); Serial.println(yTIME);
      }
      break;

    case yLED: // Đèn xanh
      if (IsReady(ledTimeStart, yTIME)) {
        digitalWrite(gLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        counter = rTIME / 1000; // Reset bộ đếm theo thời gian đèn đỏ
        Serial.print("1. RED \t\t => Next "); Serial.println(rTIME);
      }
      break;
  }
}

void Testing_Display(int timeLeft) {
  if (IsReady(displayTimeStart, 1000)) { // Cập nhật hiển thị mỗi giây
    display.showNumberDec(timeLeft, true, 2, 2);
    if (counter > 0) {
      counter--; // Giảm bộ đếm
    }
  }
}
