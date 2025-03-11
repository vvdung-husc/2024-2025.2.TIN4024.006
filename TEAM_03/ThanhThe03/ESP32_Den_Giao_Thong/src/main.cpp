#include <Arduino.h>

const int Red = 5;     // Đèn đỏ
const int Yellow = 16; // Đèn vàng
const int Green = 15;  // Đèn xanh
ulong lastChange = 0;  
int state = 0;         

void setup() {
  Serial.begin(115200);
  pinMode(Red, OUTPUT);
  pinMode(Yellow, OUTPUT);
  pinMode(Green, OUTPUT);
}
void Use_Non_Blocking(){
  ulong t = millis();

  switch (state) {
    case 0: 
      digitalWrite(Red, HIGH);
      digitalWrite(Yellow, LOW);
      digitalWrite(Green, LOW);
      Serial.println("RED -> ON");
      if (t - lastChange >= 10000) { 
        state = 1; lastChange = t; 
      }
      break;

    case 1: 
      digitalWrite(Red, LOW);
      digitalWrite(Yellow, HIGH);
      digitalWrite(Green, LOW);
      Serial.println("YELLOW -> ON");
      if (t - lastChange >= 5000) { 
        state = 2; lastChange = t; 
      }
      break;

    case 2:
      digitalWrite(Red, LOW);
      digitalWrite(Yellow, LOW);
      digitalWrite(Green, HIGH);
      Serial.println("GREEN -> ON");
      if (t - lastChange >= 10000) { 
        state = 0; lastChange = t; 
      }
      break;
  }
}
void loop() {
  Use_Non_Blocking();
}