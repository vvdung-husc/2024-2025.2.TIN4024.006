#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h> // Thêm thư viện cho HTTPS

#define BLYNK_TEMPLATE_ID "TMPL6hAbf5B8F"
#define BLYNK_TEMPLATE_NAME "ESP32 HTTP"
#define BLYNK_AUTH_TOKEN "BLHLCA00ZE4lpFJ_XZVl-YqyD7UaR6-H"

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Thông tin WiFi
char ssid[] = "Wokwi-GUEST";  //Tên mạng WiFi
char pass[] = "";             //Mật khẩu mạng WiFi

unsigned long previousMillis = 0;
const long interval = 1000;  // 1 giây



// URL API
const char* serverName = "http://ip4.iothings.vn?geo=1";
const char* API_KEY = "452bb664cf0ff0241615e3bbc2f205f9";
void requestAPI();

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

void upTime(){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float uptimeSeconds = millis() / 1000.0;  // Uptime tính bằng giây
        Blynk.virtualWrite(V0, uptimeSeconds);   // Hiển thị trên Blynk (có thể đổi V4)
        
        Serial.print("Uptime: ");
        Serial.println(uptimeSeconds);
    }
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
    Serial.print("Connecting to ");
    Blynk.begin(BLYNK_AUTH_TOKEN,ssid, pass); //Kết nối đến mạng WiFi

    Serial.println();
    Serial.println("WiFi connected");

    Serial.println("== START ==>");
    requestAPI();
    
}

void loop() {
    Blynk.run();
    upTime();
}


void requestAPI(){
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

                String api = "https://api.openweathermap.org/data/2.5/weather?lat="+String(latitude)+"&lon="+String(longitude)+"&units=metric&appid="+ String(API_KEY);
                
                WiFiClientSecure client;
                client.setInsecure(); // Bỏ qua SSL
            
                HTTPClient http;
                http.begin(client, api); // Sử dụng client bảo mật cho HTTPS
            
                Serial.println("Đang gửi request đến OpenWeatherMap...");
                int httpCode = http.GET();
            
                if (httpCode > 0) {
                    String payload = http.getString();
                    Serial.println("Dữ liệu JSON nhận được:");
                    Serial.println(payload);
            
                    // Phân tích JSON
                    StaticJsonDocument<1024> doc;
                    DeserializationError error = deserializeJson(doc, payload);
            
                    if (error) {
                        Serial.print("Lỗi parse JSON: ");
                        Serial.println(error.f_str());
                    } else {
                        float temp = doc["main"]["temp"]; // Lấy nhiệt độ
                        Serial.print("Nhiệt độ hiện tại: ");
                        Serial.print(temp);
                        Blynk.virtualWrite(V3, temp);
                        Serial.println("°C");
                    }
                } else {
                    Serial.print("Lỗi HTTP: ");
                    Serial.println(httpCode);
                }
                
                
                Serial.println("================================");
            } else {
                Serial.println("Lỗi: Chuỗi phản hồi không đúng định dạng.");
            }
        } else {
            Serial.print("Lỗi gửi request: ");
            Serial.println(httpResponseCode);
        }

        http.end();
}


