#include <Arduino.h>
#include <TM1637Display.h>

//Pin
#define rLED  5
#define yLED  17
#define gLED  16

#define CLK   23
#define DIO   22

//1000 ms = 1 seconds
#define rTIME  5000   //5 seconds
#define yTIME  2000
#define gTIME  7000

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = rLED;

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light();

void setup() {
  // put your setup code here, to run once:
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
  Serial.println("== START ==>");
  Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);
}

int counter = 9;
void Testing_Display(){
  display.showNumberDec(counter, true, 2, 2);

  counter--;
  if (counter == 0) {
    counter = 9;
  }

  delay(1000); 
}

void loop() {
  // put your main code here, to run repeatedly:
  //currentMiliseconds = millis();
  //NonBlocking_Traffic_Light();

  Testing_Display();
 
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}
void NonBlocking_Traffic_Light(){
  switch (currentLED) {
    case rLED: // Đèn đỏ: 5 giây
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        Serial.print("2. GREEN\t => Next "); Serial.println(nextTimeTotal);        
      } 
      break;

    case gLED: // Đèn xanh: 7 giây
      if (IsReady(ledTimeStart,gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        Serial.print("3. YELLOW\t => Next "); Serial.println(nextTimeTotal);        
      }
      break;

    case yLED: // Đèn vàng: 2 giây
      if (IsReady(ledTimeStart,yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        Serial.print("1. RED \t\t => Next "); Serial.println(nextTimeTotal);        
      }
      break;
  }
}
