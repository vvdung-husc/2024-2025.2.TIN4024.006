#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>

#define BLYNK_TEMPLATE_ID "TMPL65_Z2D4HM"
#define BLYNK_TEMPLATE_NAME "TramThoiTietMini"
#define BLYNK_AUTH_TOKEN "kEEi-ShHAHj4URy5mYmG0Pqp9dFxkULm"

#include <BlynkSimpleEsp32.h>

// Thông tin WiFi và Blynk
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN 14
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Công tắc (từ Blynk V0)
bool congTac = false;
BLYNK_WRITE(V0) {
  congTac = param.asInt();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Khong tim thay man hinh OLED!"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Khoi dong...");
  display.display();
  delay(1000);
}

void loop() {
  Blynk.run();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int pressure = random(950, 1051);  // hPa

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Loi cam bien DHT22!");
    return;
  }

  Serial.print("Nhiet do: ");
  Serial.print(temperature);
  Serial.print("°C | Do am: ");
  Serial.print(humidity);
  Serial.print("% | Ap suat: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  // Gửi dữ liệu đến Blynk
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, pressure);

  // Hiển thị OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Cong tac: ");
  display.println(congTac ? "ON" : "OFF");

  display.print("Nhiet do: ");
  display.print(temperature);
  display.println(" C");

  display.print("Do am: ");
  display.print(humidity);
  display.println(" %");

  display.print("Ap suat: ");
  display.print(pressure);
  display.println(" hPa");

  display.display();

  delay(2000);
}
