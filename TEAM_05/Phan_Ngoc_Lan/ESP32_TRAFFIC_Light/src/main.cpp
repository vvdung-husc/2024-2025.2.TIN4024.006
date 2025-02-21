#include <Arduino.h>
#include <TM1637Display.h>

// Định nghĩa các chân LED
#define rLED 5
#define yLED 17
#define gLED 16

// Chân module 7-segment
#define CLK 23
#define DIO 22

// Thời gian các trạng thái đèn
const unsigned long rTIME = 30000;
const unsigned long yTIME = 3000;
const unsigned long gTIME = 30000;

enum TrafficLightState
{
  RED,
  GREEN,
  YELLOW
};
TrafficLightState currentState = RED;
unsigned long ledTimeStart = 0;
int lastDisplayedTime = -1; //

TM1637Display display(CLK, DIO);

// KHAI BÁO HÀM TRƯỚC
void NonBlocking_Traffic_Light(unsigned long now);
void updateCountdown(unsigned long now);

void setup()
{
  Serial.begin(115200);

  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);

  display.setBrightness(7);

  digitalWrite(rLED, HIGH);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);

  currentState = RED;
  ledTimeStart = millis();

  Serial.println("== START ==> Đèn ĐỎ bật");
}

void loop()
{
  unsigned long now = millis();

  // Cập nhật trạng thái đèn nếu hết thời gian
  NonBlocking_Traffic_Light(now);
  // Cập nhật đồng hồ đếm ngược
  updateCountdown(now);
}

void NonBlocking_Traffic_Light(unsigned long now)
{
  unsigned long elapsed = now - ledTimeStart;

  switch (currentState)
  {
  case RED:
    if (elapsed >= rTIME)
    {
      digitalWrite(rLED, LOW);
      digitalWrite(gLED, HIGH);
      currentState = GREEN;
      ledTimeStart = now;
      Serial.println("Chuyển sang đèn XANH");
    }
    break;

  case GREEN:
    if (elapsed >= gTIME)
    {
      digitalWrite(gLED, LOW);
      digitalWrite(yLED, HIGH);
      currentState = YELLOW;
      ledTimeStart = now;
      Serial.println("Chuyển sang đèn VÀNG");
    }
    break;

  case YELLOW:
    if (elapsed >= yTIME)
    {
      digitalWrite(yLED, LOW);
      digitalWrite(rLED, HIGH);
      currentState = RED;
      ledTimeStart = now;
      Serial.println("Chuyển sang đèn ĐỎ");
    }
    break;
  }
}

void updateCountdown(unsigned long now)
{
  unsigned long stateDuration;
  if (currentState == RED)
    stateDuration = rTIME;
  else if (currentState == GREEN)
    stateDuration = gTIME;
  else
    stateDuration = yTIME;

  int secondsRemaining = (stateDuration - (now - ledTimeStart)) / 1000;

  if (secondsRemaining != lastDisplayedTime)
  {
    display.showNumberDec(secondsRemaining, true, 2, 2);
    lastDisplayedTime = secondsRemaining;
  }
}
