#include <Arduino.h>
#include <TM1637Display.h>
#include <WiFi.h>
#include <WiFiClient.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6Cqwx1Htv"
#define BLYNK_TEMPLATE_NAME "LAB"
#define BLYNK_AUTH_TOKEN "k-DFMMT_L9Mg6mjGRyuG7dfu5nmK39ok"

#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST"; // Tên mạng WiFi
char pass[] = "";            // Mật khẩu mạng WiFi

#define btnBLED 23 // Chân kết nối nút bấm
#define pinBLED 21 // Chân kết nối đèn xanh

#define CLK 18 // Chân kết nối CLK của TM1637
#define DIO 19 // Chân kết nối DIO của TM1637

#define DHTPIN 16 // Chân kết nối DHT22
#define DHTTYPE DHT22

// Biến toàn cục
ulong currentMiliseconds = 0; // Thời gian hiện tại - miliseconds
bool blueButtonON = true;     // Trạng thái của nút bấm ON -> đèn Xanh sáng và hiển thị LED TM1637
float humidity = 0.0;
float temperature = 0.0;

// Khởi tạo mà hình TM1637
TM1637Display display(CLK, DIO);
DHT dht(DHTPIN, DHTTYPE);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void updateDHT();

// Hàm nhận tín hiệu từ Blynk khi nút trên giao diện được nhấn
BLYNK_WRITE(V2) {
  blueButtonON = param.asInt(); // Nhận giá trị từ Blynk (0 hoặc 1)
  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW); // Cập nhật trạng thái đèn
  if (!blueButtonON) {
    display.clear(); // Tắt hiển thị nếu đèn tắt
  }
  Serial.print("Blynk Button State: ");
  Serial.println(blueButtonON ? "ON" : "OFF");
}

// Hàm được gọi khi thiết bị kết nối với Blynk
BLYNK_CONNECTED() {
  Serial.println("Connected to Blynk server");
  Blynk.syncVirtual(V2); // Đồng bộ trạng thái nút bấm
  Blynk.virtualWrite(V3, millis() / 1000); // Đồng bộ thời gian ban đầu
}

void setup() {
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);

  display.setBrightness(0x0f);

  // Start the WiFi connection
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");
  dht.begin();

  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V2, blueButtonON); // Đồng bộ trạng thái ban đầu lên Blynk
  Serial.println("== START ==>");
}

void loop() {
  Blynk.run();
  if (Blynk.connected()) {
    currentMiliseconds = millis();
    uptimeBlynk();
    updateBlueButton();
    updateDHT();
  } else {
    Serial.println("Blynk disconnected. Reconnecting...");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Thử kết nối lại
  }
}

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}

void updateBlueButton() {
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50))
    return;
  int v = digitalRead(btnBLED);
  if (v == lastValue)
    return;
  lastValue = v;
  if (v == LOW)
    return;

  if (!blueButtonON) {
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
  } else {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);
    blueButtonON = false;
    display.clear();
  }
  Blynk.virtualWrite(V2, blueButtonON); // Gửi trạng thái mới lên Blynk
}


void uptimeBlynk() {
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000))
    return;
  ulong value = millis() / 1000; // Tính thời gian chạy (giây)
  Blynk.virtualWrite(V3, value); // Gửi thời gian lên V3
  if (blueButtonON) {
    display.showNumberDec(value); // Hiển thị trên TM1637
  }
}

void updateDHT() {
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 2000))
    return;

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Blynk.virtualWrite(V0, temperature); // Gửi nhiệt độ lên V0
  Blynk.virtualWrite(V1, humidity);    // Gửi độ ẩm lên V1
  
  Serial.print(F("Nhiệt độ: "));
  Serial.print(temperature);
  Serial.print(F("°C, Độ ẩm: "));
  Serial.print(humidity);
  Serial.println(F("%"));
}