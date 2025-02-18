#include <Arduino.h>

int ledPinRed = 5;
int ledPinBlue = 2;
int ledPinYellow = 4;
ulong ledstart = 0;
ulong ledStart = 0;
int ledState = 0;
bool isLED_ON = false;
void setup()
{
  Serial.begin(115200);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  ulong t = millis();
  if (t - ulTimer < milisecond)
    return false;
  ulTimer = t;
  return true;
}

void UpdateLEDs()
{
  if (ledState == 0)
  {
    if (!IsReady(ledstart, 1000))
      return;

    if (!isLED_ON)
    {
      digitalWrite(ledPinRed, HIGH);
      digitalWrite(ledPinBlue, LOW);
      digitalWrite(ledPinYellow, LOW); // Bật LED
      Serial.println("NonBlocking LEDRED -> ON");
    }
    else
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinBlue, LOW);
      digitalWrite(ledPinYellow, LOW); // Tắt LED
      Serial.println("NonBlocking LEDRED -> OFF");
    }
    isLED_ON = !isLED_ON;
    if (IsReady(ledStart, 60000))
      ledState = 1;
  }
  else if (ledState == 1)
  {
    if (!IsReady(ledstart, 1000))
      return;

    if (!isLED_ON)
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinBlue, HIGH);
      digitalWrite(ledPinYellow, LOW); // Bật LED
      Serial.println("NonBlocking LEDBLUE -> ON");
    }
    else
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinBlue, LOW);
      digitalWrite(ledPinYellow, LOW); // Tắt LED
      Serial.println("NonBlocking LEDBLUE -> OFF");
    }
    isLED_ON = !isLED_ON;
    if (IsReady(ledStart, 30000))
      ledState = 2;
  }
  else if (ledState == 2)
  {
    if (!IsReady(ledstart, 1000))
      return;

    if (!isLED_ON)
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinBlue, LOW);
      digitalWrite(ledPinYellow, HIGH); // Bật LED
      Serial.println("NonBlocking LEDYELLOW -> ON");
    }
    else
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinBlue, LOW);
      digitalWrite(ledPinYellow, LOW); // Tắt LED
      Serial.println("NonBlocking LEDYELLOW -> OFF");
    }
    isLED_ON = !isLED_ON;
    if (IsReady(ledStart, 7000))
      ledState = 0;
  }
}

void loop()
{
  UpdateLEDs();
}
