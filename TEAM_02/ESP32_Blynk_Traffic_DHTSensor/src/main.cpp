#include <Arduino.h>
#include <DHT.h>
#include <TM1637Display.h>

#define BLYNK_TEMPLATE_ID "TMPL6lD14z4JL"
#define BLYNK_TEMPLATE_NAME "ESP 32 Led controll"
#define BLYNK_AUTH_TOKEN "SsfKH68GnPaMpwFo7SuSHWogtbX80vw3"

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

DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);
BlynkTimer timer;
bool systemState = true;

BLYNK_WRITE(V1) {
  systemState = param.asInt();
  digitalWrite(RELAY_PIN, systemState ? HIGH : LOW);
  Serial.print("Hệ thống: "); Serial.println(systemState ? "Bật" : "Tắt");

  // Đồng bộ trạng thái hệ thống lên Blynk
  Blynk.virtualWrite(V1, systemState);

  // Xóa màn hình nếu tắt hệ thống
  if (!systemState) {
    display.clear();
  }
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

  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V2, h);
}

void updateUptime() {
  if (!systemState) return;

  int uptime = millis() / 1000;
  
  Blynk.virtualWrite(V0, uptime);

  Serial.print("Thời gian hoạt động: ");
  Serial.print(uptime / 60); Serial.print(" phút ");
  Serial.print(uptime % 60); Serial.println(" giây");

  // Hiển thị thời gian hoạt động lên TM1637
  display.showNumberDec(uptime);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  dht.begin();
  display.setBrightness(5);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Đồng bộ trạng thái hệ thống khi khởi động
  Blynk.virtualWrite(V2, systemState);

  timer.setInterval(2000L, sendSensor); // Cập nhật nhiệt độ & độ ẩm mỗi 2 giây
  timer.setInterval(1000L, updateUptime); // Cập nhật thời gian hoạt động mỗi giây
}

void loop() {
  Blynk.run();
  timer.run();
}
