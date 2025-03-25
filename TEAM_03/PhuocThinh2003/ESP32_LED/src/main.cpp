#include <Arduino.h>
int ledPin = 5; // Chân GPIO 2 (thường nối sẵn LED trên board)

bool isLED_ON = false;
ulong ledStart = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
}

void Use_Blocking(){
  digitalWrite(ledPin, HIGH); // Bật LED
  Serial.println("LED -> ON");
  delay(1000);                // Đợi 1 giây
  digitalWrite(ledPin, LOW);  // Tắt LED
  Serial.println("LED -> OFF");
  delay(1000);                // Đợi 1 giây  
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
    digitalWrite(ledPin, HIGH); // Bật LED
    Serial.println("NonBlocking LED -> ON");  
  }
  else {
    digitalWrite(ledPin, LOW); // Tắt LED
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