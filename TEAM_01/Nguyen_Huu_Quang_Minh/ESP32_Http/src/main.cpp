#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

// Wokwi sử dụng mạng WiFi "Wokwi-GUEST" không cần mật khẩu cho việc chạy mô phỏng
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

void TachChuoi(String data){
  int start = 0, end = 0, index = 0;
  String parts[7];

  while ((end = data.indexOf('|', start)) != -1)
  {
      parts[index++] = data.substring(start, end);
      start = end + 1;
  }
  parts[index] = data.substring(start); // lấy phần cuối cùng sau dấu |
  
  Serial.println("===== Thông tin địa chỉ IP =====");
  Serial.println("Địa chỉ IP: " + parts[0]);
  Serial.println("Mã quốc gia: " + parts[1]);
  Serial.println("Quốc gia: " + parts[2]);
  Serial.println("Khu vực: " + parts[3]);
  Serial.println("Thành phố: " + parts[4]);
  Serial.print("Tọa độ: ");
  Serial.print(parts[6].toFloat());  // Latitude
  Serial.print(", ");
  Serial.println(parts[5].toFloat());  // Longitude
  Serial.println("================================");
}

void getGeoInfo() {
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = "http://ip4.iothings.vn?geo=1";
    Serial.print("Gửi yêu cầu HTTP đến: ");
    Serial.println(url);

    http.begin("ip4.iothings.vn", 80, "/?geo=1");
    int httpResponseCode = http.GET();

    if(httpResponseCode > 0){
      String payload = http.getString();
      Serial.println("Dữ liệu nhận được: ");
      Serial.println(payload);

      TachChuoi(payload);
    }else {
      Serial.print("Lỗi HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }else{
    Serial.println("Chưa kết nối WiFi!");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid,pass);

  Serial.print("Đang kết nối wifi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Đã kết nối Wifi");

  // Gửi yêu cầu HTTP và xử lý dữ liệu
  getGeoInfo();
}

void loop() {
  // Không cần code trong loop nếu chỉ thực hiện một lần
}