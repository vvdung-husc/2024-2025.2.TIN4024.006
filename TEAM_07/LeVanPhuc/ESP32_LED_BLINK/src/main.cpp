#include <Arduino.h>

int RedPin = 5;   // Chân đèn đỏ
int YelPin = 17;  // Chân đèn vàng
int GrePin = 2;   // Chân đèn xanh

enum LightState { RED, YELLOW, GREEN };
LightState currentLight = RED;
ulong lightTimer = 0;
bool isBlinkOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(RedPin, OUTPUT);
  pinMode(YelPin, OUTPUT);
  pinMode(GrePin, OUTPUT);
}

void setTrafficLight(LightState state, bool blink) {
  digitalWrite(RedPin, (state == RED) && (!blink || isBlinkOn));
  digitalWrite(YelPin, (state == YELLOW) && (!blink || isBlinkOn));
  digitalWrite(GrePin, (state == GREEN) && (!blink || isBlinkOn));
}

void updateTrafficLight() {
  ulong currentTime = millis();
  static ulong lastChange = 0;
  static ulong lastBlink = 0;
  static int duration = 5000;  // Mặc định 5 giây
  static int blinkInterval = 500;

  if (currentTime - lastBlink >= blinkInterval) {
    lastBlink = currentTime;
    isBlinkOn = !isBlinkOn;
    setTrafficLight(currentLight, true);
  }

  if (currentTime - lastChange >= duration) {
    lastChange = currentTime;
    
    switch (currentLight) {
      case RED:
        currentLight = GREEN;
        duration = 5000; // Xanh trong 5 giây
        break;
      case GREEN:
        currentLight = YELLOW;
        duration = 2000; // Vàng trong 2 giây
        break;
      case YELLOW:
        currentLight = RED;
        duration = 5000; // Đỏ trong 5 giây
        break;
    }
    setTrafficLight(currentLight, true);
    Serial.print("Traffic Light: ");
    Serial.println(currentLight == RED ? "RED" : currentLight == YELLOW ? "YELLOW" : "GREEN");
  }
}

void loop() {
  updateTrafficLight();
}