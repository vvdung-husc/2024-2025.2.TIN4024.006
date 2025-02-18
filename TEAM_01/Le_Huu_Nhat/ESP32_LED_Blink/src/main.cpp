#include <Arduino.h>

int LedDo = 5;
int LedVang = 17;
int LedXanh = 16;
bool isLED_ON = false;
ulong ledStart = 0;
int LedState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LedDo,OUTPUT);
  pinMode(LedVang,OUTPUT);
  pinMode(LedXanh,OUTPUT);
  
}

// void Use_Blocking(){
//   digitalWrite(LedDo, HIGH);
//   Serial.println("LED -> ON");
//   delay(1000);
//   digitalWrite(LedDo, LOW);
//   Serial.println("LED -> OFF");
//   delay(1000);
// }


bool IsReady(ulong& ulTimer, uint32_t millisecond){
  ulong t = millis();
  if(t - ulTimer < millisecond) return false;
  ulTimer = t;
  return true;
}

void Use_Non_Blocking() {
  unsigned long currentMillis = millis();
  switch (LedState)
  {
  case 0:
    digitalWrite(LedDo, HIGH);
    digitalWrite(LedVang, LOW);
    digitalWrite(LedXanh, LOW);
    if (IsReady(ledStart,5000)) { 
      LedState = 1;
      ledStart = currentMillis;
    }
    break;
  
  case 1:
    digitalWrite(LedDo, LOW);
    digitalWrite(LedVang, HIGH);
    digitalWrite(LedXanh, LOW);
    if (IsReady(ledStart,2000)) { 
      LedState = 2;
      ledStart = currentMillis;
    }
    break;
  case 2:
    digitalWrite(LedDo, LOW);
    digitalWrite(LedVang, LOW);
    digitalWrite(LedXanh, HIGH);
    if (IsReady(ledStart,3000)) { 
      LedState = 0;
      ledStart = currentMillis;
    }
    break;
  }
  
}

void loop() {
  // Use_Blocking();
  Use_Non_Blocking();

  ulong t = millis();
  Serial.print("Time: ");
  Serial.println(t);
}

// void loop() {
// digitalWrite(Ledpin, HIGH);
// Serial.println("LED -> ON");
// delay(1000);
// digitalWrite(Ledpin, LOW);
// Serial.println("LED -> OFF");
// delay(1000);
// }
