#include <Arduino.h>

int ledPin = 23; //chân đèn

bool isLED_ON = false;

ulong ledStart = 0;

void setup(){
  Serial.begin(115200);//Khởi tạo Serial = Serial.begin(số lượng bit);
  pinMode(ledPin,OUTPUT);//là gì ???
}

// void loop(){
//   digitalWrite(ledPin,HIGH);
//   Serial.println("Led -> ON");
//   delay(1000);
//   digitalWrite(ledPin,LOW);
//   Serial.println("Led -> OFF");
//   delay(1000);
// }
void Use_Blocking(){
  digitalWrite(ledPin,HIGH);
  Serial.println("Led -> ON");
  delay(1000);
  digitalWrite(ledPin,LOW);
  Serial.println("Led -> OFF");
  delay(1000);
}

//Đã sẵn sàng để bật/tắt đèn 
bool IsReady(ulong& ulTimer, uint32_t milisecond){
  ulong t = millis();
  if (t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}

//Lập trình vi mạch sử dụng Non_Blocking
void Use_Non_Blocking(){
  if (!IsReady(ledStart,1000)) return;

  if (!isLED_ON){
    digitalWrite(ledPin, HIGH); // Bật LED
    Serial.print("NonBlocking LED -> ON");  
  }
  else {
    digitalWrite(ledPin, LOW); // Tắt LED
    Serial.print("NonBlocking LED -> OFF");  
  }
  isLED_ON = !isLED_ON;
}

void loop(){
  // Use_Blocking();
  Use_Non_Blocking();
  ulong t = millis();
  Serial.print("Timer: ");
  Serial.println(t);
}


