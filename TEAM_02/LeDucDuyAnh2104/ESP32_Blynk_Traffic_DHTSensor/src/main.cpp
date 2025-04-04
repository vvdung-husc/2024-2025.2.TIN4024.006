#include <Arduino.h>
#include <DHT.h>
#include <TM1637Display.h>

#define BLYNK_TEMPLATE_ID "TMPL6X8Rtzm6G"
#define BLYNK_TEMPLATE_NAME "ESP32 Traffic DHT"
#define BLYNK_AUTH_TOKEN "jbZcdNcubnddZshFTaL0dwrokZoBYvd7"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN 16
#define DHTTYPE DHT22
#define CLK 18
#define DIO 19
#define RELAY_PIN 21
#define LED_PIN 21
#define BUTTON_PIN 23

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;
bool systemState = true;
bool lastButtonState = HIGH;

BLYNK_WRITE(V2) {
  systemState = param.asInt();
  Serial.print("Hệ thống: "); Serial.println(systemState ? "Bật" : "Tắt");

  if (systemState) {
    digitalWrite(LED_PIN, HIGH);  // Bật đèn LED
  } else {
    digitalWrite(LED_PIN, LOW);   // Tắt đèn LED
    display.clear();              // Tắt màn hình LED
  }

  Blynk.virtualWrite(V2, systemState);
}

void sendSensor() {
  if (!systemState) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi đọc cảm biến DHT22!");
    return;
  }

  Serial.print("Nhiệt độ: "); Serial.print(t); Serial.println("°C");
  Serial.print("Độ ẩm: "); Serial.print(h); Serial.println("%");

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void updateUptime() {
  int uptime = millis() / 1000;
  Blynk.virtualWrite(V3, uptime);

  Serial.print("Thời gian hoạt động: ");
  Serial.print(uptime / 60); Serial.print(" phút ");
  Serial.print(uptime % 60); Serial.println(" giây");

  if (systemState) {
    display.showNumberDec(uptime);
  }
}

void checkButton() {
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      systemState = !systemState;
      Serial.print("Hệ thống: "); Serial.println(systemState ? "Bật" : "Tắt");
      Blynk.virtualWrite(V2, systemState);

      if (systemState) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
        display.clear();
      }
    }
  }
  lastButtonState = buttonState;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  dht.begin();
  display.setBrightness(5);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.virtualWrite(V2, systemState);
  digitalWrite(LED_PIN, systemState ? HIGH : LOW); // Cập nhật trạng thái LED

  timer.setInterval(2000L, sendSensor);
  timer.setInterval(1000L, updateUptime);
  timer.setInterval(100L, checkButton);
}

void loop() {
  Blynk.run();
  timer.run();
}
