// Thông tin Blynk
#define BLYNK_TEMPLATE_ID "TMPL6H11GG0To"
#define BLYNK_TEMPLATE_NAME "ESP32 API"
#define BLYNK_AUTH_TOKEN "jXHrzeMwvg7why8xIzTlzAsDGReNXsKp"

#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
// URL API để lấy vị trí từ IP
const char* serverUrl = "http://ip4.iothings.vn/?geo=1";
// Thông tin WiFi
const char* ssid = "Wokwi-GUEST"; // Thay bằng tên WiFi của bạn
const char* password = ""; // Thay bằng mật khẩu WiFi của bạn

// Định nghĩa Virtual Pin trên Blynk
#define V1 1  // Uptime
#define V2 2  // IP Address
#define V3 3  // Google Maps Link

void fetchAndParseData();
void parseResponse(String payload);

void setup() {
  Serial.begin(115200);

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // Kết nối Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();

  // Cập nhật thời gian chạy (Uptime)
  Blynk.virtualWrite(V1, millis() / 1000);

  // Lấy vị trí từ IP mỗi 10 giây
  fetchAndParseData();
  delay(10000);
}

void fetchAndParseData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response: " + payload);
      parseResponse(payload);
    } else {
      Serial.printf("HTTP GET failed, error code: %d\n", httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void parseResponse(String response) {
  int firstPipe = response.indexOf('|');
  int secondPipe = response.indexOf('|', firstPipe + 1);
  int thirdPipe = response.indexOf('|', secondPipe + 1);
  int fourthPipe = response.indexOf('|', thirdPipe + 1);
  int fifthPipe = response.indexOf('|', fourthPipe + 1);
  int sixthPipe = response.indexOf('|', fifthPipe + 1);

  String ip = response.substring(0, firstPipe);
  String latitude = response.substring(fifthPipe + 1, sixthPipe);
  String longitude = response.substring(sixthPipe + 1);

  Serial.println("IP: " + ip);
  Serial.println("Latitude: " + latitude);
  Serial.println("Longitude: " + longitude);

  // Tạo đường link Google Maps
  String googleMapsLink = "https://www.google.com/maps?q=" +  longitude + "," + latitude;
  Serial.println("Google Maps Link: " + googleMapsLink);

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V2, ip);
  Blynk.virtualWrite(V3, googleMapsLink);
}
