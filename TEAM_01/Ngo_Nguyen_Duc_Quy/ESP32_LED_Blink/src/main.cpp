#include <Arduino.h>

int ledPin = 5;
bool IsLed_On = false;
ulong led_start = 0;
// put function declarations here:
int myFunction(int, int);

void setup() { // đoạn code khởi tạo
  // put your setup code here, to run once:
  Serial.begin(115200);

  int result = myFunction(2, 3);
  Serial.println("IOT welcome");
  pinMode(ledPin, OUTPUT);
}

void Use_Blocking(){
  digitalWrite(ledPin, HIGH); // Bật
  Serial.println("LED -> ON");
  delay(1000);                // Chờ 1 giây có cơ chế block
  digitalWrite(ledPin, LOW);  // tắt
  Serial.println("LED -> OFF");
  delay(1000);
}

bool iSReady(ulong& ulTimer, uint32_t milisecond){
  ulong t = millis();
  if(t - ulTimer < milisecond) return false;
  ulTimer = t;
  return true;
}

void Use_Non_Blocking(){
  if(!iSReady(led_start,1000)) return;
  if(!IsLed_On){
    digitalWrite(ledPin, HIGH); // Bật
    Serial.println("NonBlocking LED -> ON");
  }else{
    digitalWrite(ledPin, LOW); // Bật
    Serial.println("NonBlocking LED -> OFF");
  }
  IsLed_On = !IsLed_On;
}

void loop() { 
  Use_Non_Blocking();

  ulong t = millis();
  Serial.print("Timer : ");
  Serial.println(t);
}
// void loop() { // lặp mãi mãi
//   // put your main code here, to run repeatedly:
//   // static uint32_t i = 0;
//   // ++i; 
//   // delay(1000); // dừng chương trình theo mili giây
//   // ulong t = millis(); // thời gian kể từ lúc chương trình khởi động và quay lại 0 khi mà nó bị tràn
//   // Serial.print("Loop i = ");
//   // Serial.print(i);

//   // Serial.print(" Timer = ");
//   // Serial.println(t);

//   digitalWrite(ledPin, HIGH); // Bật
//   Serial.println("LED -> ON");
//   delay(1000);                // Chờ 1 giây có cơ chế block
//   digitalWrite(ledPin, LOW);  // tắt
//   Serial.println("LED -> OFF");
//   delay(1000);

// }


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}