#include <Arduino.h>

int ledPin = 5;
int ledPinYel = 16;
int ledpinGre = 4;

bool isLED_ON = false;
ulong lesStart = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinYel, OUTPUT);
  pinMode(ledpinGre, OUTPUT);

}

void Use_Blocking(){
  digitalWrite(ledPin, HIGH);
  Serial.print("LED -> ON");
  delay(1000);
  digitalWrite(ledPin,LOW);
  Serial.print("LED -> OFF");
  delay(1000);
}



// void Use_Non_Blocking()
// {
//   if(!IsReady(lesStart,100)) return;
//   if(!isLED_ON){
//     digitalWrite(ledPin, HIGH);
//     Serial.print("NonBlockingLED -> ON");
    
//   } else {
//     digitalWrite(ledPin,LOW);
//     Serial.print("NonBlockingLED -> OFF");
  
//   }
//   isLED_ON = !isLED_ON;
// }

bool IsReady(ulong& ulTimer, uint32_t milisecond){
  ulong t = millis();
  if(t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}


void Use_Non_Blocking()
{
  if(!IsReady(lesStart,100)) return;
  if(!isLED_ON){
    digitalWrite(ledPin, HIGH);
    Serial.print("NonBlockingLED -> ON");
    
  } else {
    digitalWrite(ledPin,LOW);
    Serial.print("NonBlockingLED -> OFF");
  
  }
  isLED_ON = !isLED_ON;
}


void loop(){
  // Use_Blocking();

  Use_Non_Blocking();

  ulong t = millis();
  Serial.print("Timer :");
  Serial.println(t);
}

// void loop() {

//   digitalWrite(ledPin, HIGH);
//   Serial.print("LED -> ON");
//   delay(1000);
//   digitalWrite(ledPin,LOW);
//   Serial.print("LED -> OFF");
//   delay(1000);

// }

