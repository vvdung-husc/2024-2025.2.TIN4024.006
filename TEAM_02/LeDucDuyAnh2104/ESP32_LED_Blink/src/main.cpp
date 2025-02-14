#include <Arduino.h>

const int ledPin = 5; // Chân GPIO điều khiển LED (GPIO2 là mặc định trên ESP32)

bool isLED_ON = false;
ulong ledStart = 0;
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // Khởi động Serial Monitor để kiểm tra
}

void Use_Blocking(){
  digitalWrite(ledPin, HIGH); // Bật đèn LED
  Serial.println("LED ON");
  delay(1000); // Chờ 1 giây

  digitalWrite(ledPin, LOW);  // Tắt đèn LED
  Serial.println("LED OFF");
  delay(1000); // Chờ 1 giây
}

bool IsReady(ulong& ulTimer, uint32_t milisecond){
  ulong t = millis();
  if(t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}
void Use_Non_Bloking(){
  if(!IsReady(ledStart,1000)) return;
  if(!isLED_ON){
    digitalWrite(ledPin, HIGH); // Bật đèn LED
    Serial.println("LED ON");
  }else{
    digitalWrite(ledPin, LOW);  // Tắt đèn LED
    Serial.println("LED OFF");
  }
  isLED_ON = !isLED_ON;
}
void loop(){
  // Use_Blocking();
  Use_Non_Bloking();
  ulong t = millis();
  Serial.print("Timer : ");
  Serial.println(t);
}
// void loop() {
//   digitalWrite(ledPin, HIGH); // Bật đèn LED
//   Serial.println("LED ON");
//   delay(1000); // Chờ 1 giây

//   digitalWrite(ledPin, LOW);  // Tắt đèn LED
//   Serial.println("LED OFF");
//   delay(1000); // Chờ 1 giây
// }

