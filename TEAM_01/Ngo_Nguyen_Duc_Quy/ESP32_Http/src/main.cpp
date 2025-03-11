#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

void TachChuoi(String data) {
  char buf[data.length() + 1];  // Chuyển `String` thành `char array`
  data.toCharArray(buf, sizeof(buf));

  char *token;
  const char *delim = "|";  // Dấu phân tách

  char *parts[7];  // Mảng chứa các phần tử tách ra
  int index = 0;

  token = strtok(buf, delim);
  while (token != NULL && index < 7) {
    parts[index++] = token;
    token = strtok(NULL, delim);
  }

  // Kiểm tra xem có đủ phần tử không
  if (index < 7) {
    Serial.println("Lỗi: Dữ liệu không hợp lệ!");
    return;
  }

  String latitude1 = (parts[6]);  // Chuyển thành số thực
  String longitude1 = (parts[5]); 
  float latitude = atof(parts[6]);  // Chuyển thành số thực
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
  Serial.println("°");

  // 🔥 Xuất link Google Maps
  Serial.print("Xem trên Google Maps: ");
  Serial.println("https://www.google.com/maps/place/" + String(latitude1) + "," + String(longitude1));

  Serial.println("--------------------------------");
}

void getGeoInfo() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://ip4.iothings.vn?geo=1";
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

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  Serial.print("Đang kết nối WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Đã kết nối WiFi");

  getGeoInfo();
}

void loop() {
}
