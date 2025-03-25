#include <Arduino.h>
#include <TM1637Display.h>

// Pin
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

// 1000 ms = 1 seconds
#define rTIME  5000   // 5 seconds
#define yTIME  2000
#define gTIME  7000

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = rLED;
int counter = 0;
ulong lastDisplayUpdate = 0; // Biến để lưu thời gian cập nhật màn hình cuối cùng

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();

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
  counter = 5; // Đặt số đếm cho đèn đỏ
  display.showNumberDec(counter, true, 2, 0);
  nextTimeTotal += rTIME;
  Serial.println("== START ==>");

  // In thông tin ban đầu
  Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);
}

void loop() {
  currentMiliseconds = millis();
  
  // Cập nhật số đếm chỉ khi có thay đổi (mỗi giây)
  if (currentMiliseconds - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = currentMiliseconds;
    NonBlocking_Traffic_Light();
  }
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void NonBlocking_Traffic_Light() {
  int timeElapsed = 0;
  int totalTime = 0;
  
  switch (currentLED) {
    case rLED: // Đèn đỏ: 5 giây
      timeElapsed = currentMiliseconds - ledTimeStart;
      totalTime = rTIME;
      if (timeElapsed >= totalTime) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        ledTimeStart = currentMiliseconds;
        counter = gTIME / 1000; // Đặt số đếm khi đèn đỏ tắt và đèn xanh sáng
        display.showNumberDec(counter, true, 2, 0);
        nextTimeTotal = gTIME / 1000;
        Serial.print("2. GREEN\t => Next "); Serial.println(nextTimeTotal);        
      } else {
        // Giảm số đếm cho đèn đỏ
        counter = max(0L, (long)(rTIME / 1000 - timeElapsed / 1000));
        display.showNumberDec(counter, true, 2, 0);
      }
      break;

    case gLED: // Đèn xanh: 7 giây
      timeElapsed = currentMiliseconds - ledTimeStart;
      totalTime = gTIME;
      if (timeElapsed >= totalTime) {
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        ledTimeStart = currentMiliseconds;
        counter = yTIME / 1000; // Đặt số đếm khi đèn xanh tắt và đèn vàng sáng
        display.showNumberDec(counter, true, 2, 0);
        nextTimeTotal = yTIME / 1000;
        Serial.print("3. YELLOW\t => Next "); Serial.println(nextTimeTotal);        
      } else {
        // Giảm số đếm cho đèn xanh
        counter = max(0L, (long)(gTIME / 1000 - timeElapsed / 1000));
        display.showNumberDec(counter, true, 2, 0);
      }
      break;

    case yLED: // Đèn vàng: 2 giây
      timeElapsed = currentMiliseconds - ledTimeStart;
      totalTime = yTIME;
      if (timeElapsed >= totalTime) {
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        ledTimeStart = currentMiliseconds;
        counter = rTIME / 1000; // Đặt số đếm khi đèn vàng tắt và đèn đỏ sáng
        display.showNumberDec(counter, true, 2, 0);
        nextTimeTotal = rTIME / 1000;
        Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);        
      } else {
        // Giảm số đếm cho đèn vàng
        counter = max(0L, (long)(yTIME / 1000 - timeElapsed / 1000));
        display.showNumberDec(counter, true, 2, 0);
      }
      break;
  }
}