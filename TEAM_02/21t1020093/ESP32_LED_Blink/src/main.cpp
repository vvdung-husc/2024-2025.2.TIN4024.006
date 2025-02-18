#include <Arduino.h>

#define RED_LED 5     
#define YELLOW_LED 16 
#define GREEN_LED 4   

ulong previousMillis = 0;
int state = 0;
int countdown = 0; // Biến đếm ngược

void setup() {
  Serial.begin(115200); 
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Bắt đầu với đèn đỏ
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  countdown = 5; // Đèn đỏ ban đầu sáng trong 5 giây
}

void loop() {
  ulong currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) { 
    previousMillis = currentMillis;
    
    // Giảm thời gian đếm ngược
    if (countdown > 0) {
      Serial.print("Thời gian còn lại: ");
      Serial.print(countdown);
      Serial.println(" giây");
      countdown--;
    }
  }

  if (state == 0 && countdown == 0) {  // Chuyển sang đèn xanh
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    state = 1;
    countdown = 5;
    previousMillis = currentMillis;
  } 
  else if (state == 1 && countdown == 0) {  // Chuyển sang đèn vàng
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    state = 2;
    countdown = 2;
    previousMillis = currentMillis;
  } 
  else if (state == 2 && countdown == 0) {  // Quay lại đèn đỏ
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    state = 0;
    countdown = 5;
    previousMillis = currentMillis;
  }
}



// int ledPin = 5;

// bool isLED_ON = false;
// ulong lesStart = 0;

// void setup() {
//   Serial.begin(115200);
//   pinMode(ledPin, OUTPUT);


// }

// void Use_Blocking(){
//   digitalWrite(ledPin, HIGH);
//   Serial.print("LED -> ON");
//   delay(1000);
//   digitalWrite(ledPin,LOW);
//   Serial.print("LED -> OFF");
//   delay(1000);
// }
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

// bool IsReady(ulong& ulTimer, uint32_t milisecond){
//   ulong t = millis();
//   if(t - ulTimer < milisecond) return false;
//   ulTimer = t;
//   return true;
// }


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


// void loop(){
//   // Use_Blocking();

//   Use_Non_Blocking();

//   ulong t = millis();
//   Serial.print("Timer :");
//   Serial.println(t);
// }

// void loop() {

//   digitalWrite(ledPin, HIGH);
//   Serial.print("LED -> ON");
//   delay(1000);
//   digitalWrite(ledPin,LOW);
//   Serial.print("LED -> OFF");
//   delay(1000);

// }

