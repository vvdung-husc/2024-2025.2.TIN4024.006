#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL61LSshM_3"
#define BLYNK_TEMPLATE_NAME "Esp32 Blynk Traffic DHT"
#define BLYNK_AUTH_TOKEN "k1veMQ7K2I6yf58j_WPx1v7oLhvTZ7SX"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi thông tin
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Chân kết nối phần cứng
#define btnBLED  23  // Chân nút nhấn
#define pinBLED  21  // Chân LED xanh
#define CLK 18       // Chân CLK của TM1637
#define DIO 19       // Chân DIO của TM1637
#define DHTPIN 16    // Chân cảm biến DHT22
#define DHTTYPE DHT22 // Kiểu cảm biến DHT22

// Khởi tạo đối tượng
TM1637Display display(CLK, DIO);
DHT dht(DHTPIN, DHTTYPE);

ulong currentMiliseconds = 0;
bool blueButtonON = true;

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void readDHT22();

void setup() {
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);
  
  display.setBrightness(0x0f);
  dht.begin();

  Serial.print("Connecting to "); Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("WiFi connected");

  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V1, blueButtonON); // Đồng bộ trạng thái LED với Blynk

  Serial.println("== START ==>");
}

void loop() {  
  Blynk.run();
  currentMiliseconds = millis();

  uptimeBlynk();
  updateBlueButton();
  readDHT22();
}

// Kiểm tra khoảng thời gian
bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

// Xử lý nút nhấn
void updateBlueButton() {
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50)) return;
  
  int v = digitalRead(btnBLED);
  if (v == lastValue) return;
  lastValue = v;
  if (v == LOW) return;

  blueButtonON = !blueButtonON;
  Serial.println(blueButtonON ? "Blue Light ON" : "Blue Light OFF");
  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V1, blueButtonON);

  if (!blueButtonON) display.clear();
}

// Gửi thời gian chạy lên Blynk & hiển thị trên TM1637
void uptimeBlynk() {
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000)) return;

  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value);

  if (blueButtonON) display.showNumberDec(value);
}

// Đọc nhiệt độ & độ ẩm từ DHT22
void readDHT22() {
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 2000)) return;  // Đọc mỗi 2 giây

  float temp = dht.readTemperature();  // Đọc nhiệt độ (Celsius)
  float hum = dht.readHumidity();      // Đọc độ ẩm (%)

  if (isnan(temp) || isnan(hum)) {
    Serial.println("Lỗi! Không đọc được dữ liệu từ DHT22");
    return;
  }

  Serial.print("Nhiệt độ: "); Serial.print(temp); Serial.print("°C  ");
  Serial.print("Độ ẩm: "); Serial.print(hum); Serial.println("%");

  Blynk.virtualWrite(V2, temp); // Gửi nhiệt độ lên Blynk
  Blynk.virtualWrite(V3, hum);  // Gửi độ ẩm lên Blynk

  if (blueButtonON) display.showNumberDec(temp * 10, true);  // Hiển thị nhiệt độ trên TM1637
}

// Nhận dữ liệu từ Blynk
BLYNK_WRITE(V1) {
  blueButtonON = param.asInt();
  Serial.println(blueButtonON ? "Blynk -> Blue Light ON" : "Blynk -> Blue Light OFF");
  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);

  if (!blueButtonON) display.clear();
}