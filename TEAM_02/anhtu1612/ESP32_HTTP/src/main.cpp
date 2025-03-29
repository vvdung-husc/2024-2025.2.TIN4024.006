#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>

#define BLYNK_TEMPLATE_ID "TMPL6rT1WI8qy"
#define BLYNK_TEMPLATE_NAME "WEATHER"
#define BLYNK_AUTH_TOKEN "PN8EJhDdV7lxx-DANu-WYHMYNx_M0FK4"

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
// Thông tin WiFi
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

ulong currentMiliseconds = 0;
bool blueButtonON = true; 

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void uptimeBlynk();
// URL API
const char* serverName = "http://ip4.iothings.vn?geo=1";

// Hàm tách chuỗi theo ký tự '|'
void splitString(String data, String output[], int maxParts, char delimiter = '|') {
    int startIndex = 0;
    int endIndex = 0;
    int partIndex = 0;

    while ((endIndex = data.indexOf(delimiter, startIndex)) != -1 && partIndex < maxParts - 1) {
        output[partIndex++] = data.substring(startIndex, endIndex);
        startIndex = endIndex + 1;
    }
    output[partIndex] = data.substring(startIndex); // Lưu phần cuối cùng
}

// Hàm chuyển đổi tọa độ thập phân sang DMS (Độ, Phút, Giây)
String convertToDMS(float coord, char pos, char neg) {
    char buf[20];
    char direction = (coord >= 0) ? pos : neg;
    coord = abs(coord);
    int degrees = (int)coord;
    float minutesFloat = (coord - degrees) * 60;
    int minutes = (int)minutesFloat;
    float seconds = (minutesFloat - minutes) * 60;
    snprintf(buf, sizeof(buf), "%d°%d'%.1f\"%c", degrees, minutes, seconds, direction);
    return String(buf);
}
bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond) return false;
  ulTimer = currentMiliseconds;
  return true;
}

void uptimeBlynk(){
    static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000)) return; //Kiểm tra và cập nhật lastTime sau mỗi 1 giây
    ulong value = lastTime / 1000;
    Blynk.virtualWrite(V0, value);  //Gửi giá trị lên chân ảo V0 trên ứng dụng Blynk.
  }

void setup() {
    Serial.begin(115200);
    
    // Kết nối WiFi
    WiFi.begin(ssid, pass);
    Serial.print("Đang kết nối WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi đã kết nối!");
    // Start the WiFi connection
  Serial.print("Connecting to ");Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

  Serial.println();
  Serial.println("WiFi connected");

  Serial.println("== START ==>");
}

void loop() {
    Blynk.run();
    currentMiliseconds = millis();
    uptimeBlynk();
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        String url = String(serverName);

        // Loại bỏ "http://"
        url.replace("http://", "");

        // Tìm vị trí dấu '?'
        int queryIndex = url.indexOf('?');

        // Tách phần tên miền và tham số
        String domain = url.substring(0, queryIndex);
        String query = "/" + url.substring(queryIndex);

        http.begin(domain,80,query);
        
        int httpResponseCode = http.GET();
        
        if (httpResponseCode > 0) {
            String payload = http.getString();  // Lấy dữ liệu chuỗi
            Serial.println("Phản hồi từ server:");
            Serial.println(payload);

            // Mảng lưu trữ kết quả tách chuỗi (7 phần tử)
            String result[7];  
            splitString(payload, result, 7);

            // Kiểm tra nếu mảng tách đủ phần tử
            if (result[6] != "") {
                String latitude = result[6];  // Latitude
                String longitude = result[5]; // Longitude

                float lati = latitude.toFloat();
                float longi = longitude.toFloat();

                String latitudeDMS = convertToDMS(lati, 'N', 'S');
                String longitudeDMS = convertToDMS(longi, 'E', 'W');

                Serial.println("===== Thông tin địa chỉ IP =====");
                Serial.println("Địa chỉ IP: " + result[0]);
                Serial.println("Mã quốc gia: " + result[1]);
                Serial.println("Quốc gia: " + result[2]);
                Serial.println("Khu vực: " + result[3]);
                Serial.println("Thành phố: " + result[4]);
                Serial.println("Latitude: " + result[6]);
                Serial.println("Longitude: " + result[5]);
                Serial.print("Tọa độ (DMS): ");
                Serial.print(latitudeDMS); 
                Serial.print(", ");
                Serial.println(longitudeDMS);
                Blynk.virtualWrite(V1, result[0]);
                // Tạo link Google Maps
                Serial.println("Google Maps: http://www.google.com/maps/place/" + String(latitude) + "," + String(longitude));
                String gg = "http://www.google.com/maps/place/"+String(latitude) + "," + String(longitude);
                Blynk.virtualWrite(V2, gg);
                Serial.println("================================");
            } else {
                Serial.println("Lỗi: Chuỗi phản hồi không đúng định dạng.");
            }
        } else {
            Serial.print("Lỗi gửi request: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Mất kết nối WiFi!");
    }

    delay(10000); // Gửi request mỗi 10 giây
}
