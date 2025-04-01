#include <Arduino.h>
#include <TM1637Display.h>

// phúc


// Khai báo chân kết nối TM1637 (CLK và DIO)
#define CLK  0
#define DIO  2

// Pin đèn giao thông
#define rLED  5
#define yLED  17
#define gLED  16

// Thời gian tương ứng (ms)
#define rTIME  5000   // 5s
#define yTIME  2000   // 2s
#define gTIME  7000   // 7s

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = rLED;
int countdown = rTIME / 1000;  // Đếm ngược thời gian theo giây

// Khởi tạo module TM1637
TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();

void setup() {
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);

  display.setBrightness(7);  // Độ sáng tối đa
  display.showNumberDec(countdown);  // Hiển thị số giây ban đầu

  currentLED = rLED;
  nextTimeTotal += rTIME;
  Serial.println("== START ==>");
  Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);
}

void loop() {
  currentMiliseconds = millis();
  
  static ulong lastUpdate = 0;
  if (millis() - lastUpdate >= 1000) { // Cập nhật đếm ngược mỗi giây
    lastUpdate = millis();
    if (countdown > 0) countdown--; 
    display.showNumberDec(countdown);
  }
  
  NonBlocking_Traffic_Light();
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
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        countdown = gTIME / 1000;
        Serial.print("2. GREEN\t => Next "); Serial.println(nextTimeTotal);
      }
      break;

    case gLED: // Đèn xanh
      if (IsReady(ledTimeStart, gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        countdown = yTIME / 1000;
        Serial.print("3. YELLOW\t => Next "); Serial.println(nextTimeTotal);
      }
      break;

    case yLED: // Đèn vàng
      if (IsReady(ledTimeStart, yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        countdown = rTIME / 1000;
        Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);
      }
      break;
  }
}
