#include <Arduino.h>
#include <TM1637Display.h>

//Pin
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

//1000 ms = 1 second
#define rTIME  5000   //5 seconds
#define yTIME  2000
#define gTIME  5000

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong blinkTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = rLED;
int countdown = rTIME / 1000;
bool transitionReady = false;
bool ledState = true;

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();
void BlinkLED();

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
  nextTimeTotal += rTIME;
  countdown = rTIME / 1000;
  transitionReady = false;
  Serial.println("== START ==>");
  Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);
}

void loop() {
  currentMiliseconds = millis();
  NonBlocking_Traffic_Light();
  BlinkLED();
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void BlinkLED() {
  if (IsReady(blinkTimeStart, 500)) { 
    ledState = !ledState;
    digitalWrite(currentLED, ledState);
    
   
    uint8_t segmentColor = (currentLED == rLED) ? 0b01000000 : (currentLED == gLED) ? 0b00100000 : 0b00010000;
    display.showNumberDecEx(countdown, segmentColor, true, 2, 2);
    
    
    if (ledState) {
      if (countdown > 0) countdown--;
      else transitionReady = true;
    }
  }
}

void NonBlocking_Traffic_Light(){
  if (!transitionReady) return;

  switch (currentLED) {
    case rLED:
      digitalWrite(rLED, LOW);
      digitalWrite(gLED, HIGH);
      currentLED = gLED;
      countdown = gTIME / 1000;
      break;

    case gLED:
      digitalWrite(gLED, LOW);
      digitalWrite(yLED, HIGH);
      currentLED = yLED;
      countdown = yTIME / 1000;
      break;

    case yLED:
      digitalWrite(yLED, LOW);
      digitalWrite(rLED, HIGH);
      currentLED = rLED;
      countdown = rTIME / 1000;
      break;
  }

  transitionReady = false;
  ledTimeStart = millis();
  Serial.print((currentLED == rLED) ? "1. RED \t\t => Next " : (currentLED == gLED) ? "2. GREEN\t => Next " : "3. YELLOW\t => Next ");
  Serial.println(nextTimeTotal);
}
