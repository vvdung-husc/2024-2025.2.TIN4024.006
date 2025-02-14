#include <Arduino.h>

// int ledPin = 5; // Chân GPIO 2 (thường nối sẵn LED trên board)

// bool isLED_ON = false;
// ulong ledStart = 0;

// void setup() {
//   Serial.begin(115200);
//   pinMode(ledPin, OUTPUT);
// }

// void Use_Blocking(){
//   digitalWrite(ledPin, HIGH); // Bật LED
//   Serial.println("LED -> ON");
//   delay(1000);                // Đợi 1 giây
//   digitalWrite(ledPin, LOW);  // Tắt LED
//   Serial.println("LED -> OFF");
//   delay(1000);                // Đợi 1 giây  
// }


// bool IsReady(ulong& ulTimer, uint32_t milisecond){
//   ulong t = millis();
//   if (t - ulTimer < milisecond) return false;
//   ulTimer = t;
//   return true;
// }

// void Use_Non_Blocking(){
//   if (!IsReady(ledStart,1000)) return;

//   if (!isLED_ON){
//     digitalWrite(ledPin, HIGH); // Bật LED
//     Serial.println("NonBlocking LED -> ON");  
//   }
//   else {
//     digitalWrite(ledPin, LOW); // Tắt LED
//     Serial.println("NonBlocking LED -> OFF");  
//   }
//   isLED_ON = !isLED_ON;
// }

// void loop() {
//   //Use_Blocking();
//   Use_Non_Blocking();

//   ulong t = millis();
//   Serial.print("Timer :");
//   Serial.println(t);
// }

// void loop() {
//   digitalWrite(ledPin, HIGH); // Bật LED
//   Serial.println("LED -> ON");
//   delay(1000);                // Đợi 1 giây
//   digitalWrite(ledPin, LOW);  // Tắt LED
//   Serial.println("LED -> OFF");
//   delay(1000);                // Đợi 1 giây
// }
const int redLed = 5;    // Đèn đỏ
const int yellowLed = 4; // Đèn vàng
const int greenLed = 2;  // Đèn xanh

unsigned long previousMillis = 0;
int state = 0;

void setup() {
  Serial.begin(115200);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  Serial.print("Timer :");
  Serial.println(currentMillis);
  
  if (currentMillis - previousMillis >= 3000) { // Chuyển trạng thái sau mỗi 3 giây
    previousMillis = currentMillis;
    state = (state + 1) % 3;

    if (state == 0) {
      digitalWrite(redLed, HIGH);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, LOW);
    } else if (state == 1) {
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, HIGH);
    } else {
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);
    }
  }
  
}

