#include <WiFi.h>
#include <HTTPClient.h>

// Thông tin mạng WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* serverUrl = "http://ip4.iothings.vn/?geo=1";
void fetchData() {
  if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      int httpResponseCode = http.GET();
      
      if (httpResponseCode > 0) {
          String payload = http.getString();
          Serial.println("Dữ liệu nhận được:");
          Serial.println(payload);
          
          // Tách dữ liệu theo dấu '|'
          int index = 0;
          String data[7];
          int start = 0;
          for (int i = 0; i < payload.length(); i++) {
              if (payload[i] == '|' || i == payload.length() - 1) {
                  data[index] = payload.substring(start, i + (i == payload.length() - 1 ? 1 : 0));
                  start = i + 1;
                  index++;
                  if (index >= 7) break;
              }
          }
          
          // In kết quả đã tách
          Serial.println("Thông tin phân tích:");
          Serial.print("IP: "); Serial.println(data[0]);
          Serial.print("Mã quốc gia: "); Serial.println(data[1]);
          Serial.print("Quốc gia: "); Serial.println(data[2]);
          Serial.print("Tỉnh/Thành phố: "); Serial.println(data[3]);
          Serial.print("Thành phố: "); Serial.println(data[4]);
          Serial.print("Kinh độ: "); Serial.println(data[5]);
          Serial.print("Vĩ độ: "); Serial.println(data[6]);
          
          // Tạo đường dẫn Google Maps
          String googleMapsUrl = "https://www.google.com/maps?q=" + data[6] + "," + data[5];
          Serial.print("Google Maps: "); Serial.println(googleMapsUrl);
      } else {
          Serial.print("Lỗi HTTP: ");
          Serial.println(httpResponseCode);
      }
      
      http.end();
  } else {
      Serial.println("Mất kết nối WiFi!");
  }
}
void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nKết nối WiFi thành công!");
    fetchData();
}

void loop() {
    // Gửi yêu cầu HTTP mỗi 10 giây
    fetchData();
    delay(10000);
}


