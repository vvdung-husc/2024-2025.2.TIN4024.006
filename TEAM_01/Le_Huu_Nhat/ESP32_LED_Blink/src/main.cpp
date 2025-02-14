#include <Arduino.h>

int Ledpin = 5;
bool isLED_ON = false;
ulong ledStart = 0;

void setup() {
  Serial.begin(115200);
  pinMode(Ledpin,OUTPUT);
  
}

void Use_Blocking(){
  digitalWrite(Ledpin, HIGH);
  Serial.println("NON_Blocking LED -> ON");
  delay(1000);
  digitalWrite(Ledpin, LOW);
  Serial.println("NON_Blockin LED -> OFF");
  delay(1000);
}

bool IsReady(ulong& ulTimer, uint32_t millisecond){
  ulong t = millis();
  if(t - ulTimer < millisecond) return false;
  ulTimer = t;
  return true;
}

void Use_Non_Blocking() {
  if(!IsReady(ledStart,1000)) return;
  if(!isLED_ON){
    digitalWrite(Ledpin, HIGH);
    Serial.println("LED -> ON");
  }
  else{
    digitalWrite(Ledpin, LOW);
    Serial.println("LED -> OFF");
  }
  isLED_ON = !isLED_ON;
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
