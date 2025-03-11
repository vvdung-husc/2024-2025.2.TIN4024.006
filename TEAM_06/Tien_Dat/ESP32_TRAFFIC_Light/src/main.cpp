#include <Arduino.h>
#include <TM1637Display.h>

// Pin định nghĩa
#define rLED  5
#define yLED  17
#define gLED  16
#define blueLED 18 // Đèn LED xanh (blue)
#define CLK   15
#define DIO   2
#define ldrPIN  13
#define btnPIN  23  // Nút bấm

// Thời gian đèn
uint rTIME = 5000;
uint yTIME = 3000;
uint gTIME = 10000;

ulong currentMiliseconds = 0;
ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = 0;
int tmCounter = rTIME / 1000;
ulong counterTime = 0;
bool screenOn = true; // Biến kiểm tra trạng thái màn hình

int lightThreshold = 500; // Ngưỡng ánh sáng (lux) để bật/tắt đèn xanh
TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void NonBlocking_Traffic_Light_TM1637();
bool isDark();
void YellowLED_Blink();
void handleButton();
void handleBlueLED();

void setup() {
  Serial.begin(115200);
  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(ldrPIN, INPUT);
  pinMode(btnPIN, INPUT_PULLUP);
  
  tmCounter = (rTIME / 1000) - 1;
  display.setBrightness(7);
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  display.showNumberDec(tmCounter--, true, 2, 2);
  
  currentLED = rLED;
  nextTimeTotal += rTIME;    
  Serial.println("== START ==>");  
}

void loop() {
  currentMiliseconds = millis();
  handleButton();
  handleBlueLED();
  
  if (isDark()) YellowLED_Blink();
  else NonBlocking_Traffic_Light_TM1637();
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void NonBlocking_Traffic_Light_TM1637() {  
  bool bShowCounter = false;
  switch (currentLED) {
    case rLED:
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        tmCounter = (gTIME / 1000) - 1;
        bShowCounter = true;  
        counterTime = currentMiliseconds;        
      }
      break;
    case gLED:
      if (IsReady(ledTimeStart, gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        tmCounter = (yTIME / 1000) - 1;
        bShowCounter = true;   
        counterTime = currentMiliseconds;    
      }
      break;
    case yLED:
      if (IsReady(ledTimeStart, yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        tmCounter = (rTIME / 1000) - 1;
        bShowCounter = true;       
        counterTime = currentMiliseconds;        
      }
      break;
  }
  if (!bShowCounter) bShowCounter = IsReady(counterTime, 1000);
  if (bShowCounter && screenOn) {
    display.showNumberDec(tmCounter--, true, 2, 2);
  } else if (!screenOn) {
    display.clear();
  }
}

bool isDark() {
  static ulong darkTimeStart = 0;
  static uint16_t lastValue = 0;
  static bool bDark = false;

  if (!IsReady(darkTimeStart, 50)) return bDark;
  uint16_t value = analogRead(ldrPIN);
  if (value == lastValue) return bDark;

  bDark = value < lightThreshold;
  lastValue = value;  
  return bDark;
}

void YellowLED_Blink() {
  static ulong yLedStart = 0;
  static bool isON = false;

  if (!IsReady(yLedStart, 1000)) return;
  digitalWrite(yLED, isON ? LOW : HIGH);
  isON = !isON;
}

void handleButton() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(btnPIN);
  
  if (currentState == LOW && lastState == HIGH) {
    screenOn = !screenOn;
    if (!screenOn) display.clear();
    delay(300);
  }
  lastState = currentState;
}

void handleBlueLED() {
  int lightValue = analogRead(ldrPIN);
  if (lightValue < lightThreshold) {
    digitalWrite(blueLED, HIGH); 
    digitalWrite(blueLED, LOW);  
  }
}
