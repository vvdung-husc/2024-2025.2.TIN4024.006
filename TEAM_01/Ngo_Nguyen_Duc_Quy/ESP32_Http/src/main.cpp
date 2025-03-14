#define BLYNK_TEMPLATE_ID "TMPL6bjAR67yg"
#define BLYNK_TEMPLATE_NAME "ESP32 Http"
#define BLYNK_AUTH_TOKEN "PBFaD9ZL3e4gXvUiB7kNJBkiz1FQVvEv"
#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino.h>



char ssid[] = "Wokwi-GUEST";  // WiFi SSID
char pass[] = "";             // WiFi Password

BlynkTimer timer;
long uptime = 0; // Biến đếm thời gian hoạt động (giây)

void TachChuoi(String data) {
  char buf[data.length() + 1];  
  data.toCharArray(buf, sizeof(buf));

  char *token;
  const char *delim = "|";  

  char *parts[7];  
  int index = 0;

  token = strtok(buf, delim);
  while (token != NULL && index < 7) {
    parts[index++] = token;
    token = strtok(NULL, delim);
  }

  if (index < 7) {
    Serial.println("Lỗi: Dữ liệu không hợp lệ!");
    return;
  }

  String latitude1 = (parts[6]);  
  String longitude1 = (parts[5]); 
  float latitude = atof(parts[6]);  
  float longitude = atof(parts[5]); 

  Serial.println("------Thông tin địa chỉ IP------");
  Serial.println("Địa chỉ IP: " + String(parts[0]));
  Serial.println("Mã quốc gia: " + String(parts[1]));
  Serial.println("Quốc gia: " + String(parts[2]));
  Serial.println("Khu vực: " + String(parts[3]));
  Serial.println("Thành phố: " + String(parts[4]));

  Serial.print("Toạ độ: ");
  Serial.print(latitude);
  Serial.print("°, ");
  Serial.print(longitude);
  Serial.println("'");

  String googleMapsLink = "https://www.google.com/maps/place/" + latitude1 + "," + longitude1;
  Serial.print("Xem trên Google Maps: ");
  Serial.println(googleMapsLink);

  Serial.println("--------------------------------");

  //  Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V1, String(parts[0]));  // Gửi IP lên Blynk V1
  Blynk.virtualWrite(V2, googleMapsLink);    // Gửi Link Google Maps lên Blynk V2
}

void getGeoInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://ip4.iothings.vn/?geo=1";
    Serial.print("Gửi yêu cầu HTTP đến: ");
    Serial.println(url);

    http.begin("http://ip4.iothings.vn/?geo=1");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Dữ liệu nhận được:");
      Serial.println(payload);

      TachChuoi(payload);
    } else {
      Serial.print("Lỗi HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Chưa kết nối WiFi!");
  }
}

void sendUptime() {
  uptime++;
  Blynk.virtualWrite(V0, uptime);  // Gửi thời gian hoạt động lên Blynk
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Đã kết nối WiFi");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Kết nối Blynk

  getGeoInfo();

  timer.setInterval(1000L, sendUptime);  // Cập nhật uptime mỗi giây
}

void loop() {
  Blynk.run();
  timer.run();
}
