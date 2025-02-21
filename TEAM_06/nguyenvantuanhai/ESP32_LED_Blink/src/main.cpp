#include <Arduino.h>
#include <TM1637Display.h>

#define rLED  5
#define yLED  17
#define gLED  16
#define CLK   23
#define DIO   22

#define rTIME  5000
#define yTIME  4000
#define gTIME  10000

TM1637Display display(CLK, DIO);

ulong ledTimeStart, lastUpdate;
int currentLED = rLED;
int timeLeft = rTIME / 1000;
bool firstRun = true;

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (millis() - ulTimer < milisecond) return false;
  ulTimer = millis();
  return true;
}

void UpdateCountdown() {
  if (millis() - lastUpdate >= 1000) { 
    lastUpdate = millis();
    display.showNumberDec(timeLeft = max(0, timeLeft - 1), true, 2, 2);
  }
}

void ChangeLight(int newLED, int newTime) {
  digitalWrite(currentLED, LOW);
  digitalWrite(newLED, HIGH);
  currentLED = newLED;
  timeLeft = newTime / 1000;
  Serial.print("Chuyển sang đèn: ");
  if (newLED == rLED) Serial.println("Đỏ");
  else if (newLED == gLED) Serial.println("Xanh");
  else if (newLED == yLED) Serial.println("Vàng");
}

void NonBlocking_Traffic_Light() {
  UpdateCountdown();
  if (currentLED == rLED && IsReady(ledTimeStart, rTIME)) ChangeLight(gLED, gTIME);
  else if (currentLED == gLED && IsReady(ledTimeStart, gTIME)) ChangeLight(yLED, yTIME);
  else if (currentLED == yLED && IsReady(ledTimeStart, yTIME)) ChangeLight(rLED, rTIME);
}

void setup() {
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  display.setBrightness(7);
  ChangeLight(rLED, rTIME);
  ledTimeStart = millis();
  Serial.println("== START ==> Đèn giao thông hoạt động!");
}

void loop() {
  NonBlocking_Traffic_Light();
}
