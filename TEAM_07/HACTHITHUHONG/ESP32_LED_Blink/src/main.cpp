#include <Arduino.h>
bool isLED_ON=false;
ulong ledStart = 0; 
void setup() {
  // Thiết lập GPIO 2 là chân OUTPUT
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
}
void Use_Blocking(){
  // Bật đèn LED
  digitalWrite(5, HIGH);
  Serial.println("LED-->ON");
  delay(5000); // Chờ 1 giây
  
  // Tắt đèn LED
  digitalWrite(5, LOW);
  Serial.println("LED-->OFF");
  delay(1000); // Chờ 1 giây

  // Bật đèn LED
  digitalWrite(17, HIGH);
  Serial.println("LED-->ON");
  delay(10000); // Chờ 1 giây
  
  // Tắt đèn LED
  digitalWrite(17, LOW);
  Serial.println("LED-->OFF");
  delay(1000); // Chờ 1 giây

  // Bật đèn LED
  digitalWrite(16, HIGH);
  Serial.println("LED-->ON");
  delay(15000); // Chờ 1 giây
  
  // Tắt đèn LED
  digitalWrite(16, LOW);
  Serial.println("LED-->OFF");
  delay(1000); // Chờ 1 giây
}
bool IsReady(ulong& ulTimer, uint32_t milisecond){
  ulong t = millis();
  if (t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}
void Use_Non_Blocking(){
  if (!IsReady(ledStart,1000)) return;

  if (!isLED_ON){
    digitalWrite(5, HIGH); // Bật LED
    Serial.println("NonBlocking LED -> ON");  
  }
  else {
    digitalWrite(5, LOW); // Tắt LED
    Serial.println("NonBlocking LED -> OFF");  
  }
  isLED_ON = !isLED_ON;
}

void loop() {
  //Use_Blocking();
  Use_Non_Blocking();

  ulong t = millis();
  Serial.print("Timer :");
  Serial.println(t);
}

// void loop() {
//   digitalWrite(ledPin, HIGH); // Bật LED
//   Serial.println("LED -> ON");
//   delay(1000);                // Đợi 1 giây
//   digitalWrite(ledPin, LOW);  // Tắt LED
//   Serial.println("LED -> OFF");
//   delay(1000);                // Đợi 1 giây
// }}
