#include <Arduino.h>
#define BLYNK_TEMPLATE_ID "TMPL6KhVK1kLG"
#define BLYNK_TEMPLATE_NAME "getAPI"
#define BLYNK_AUTH_TOKEN "KjWx46BYfGTnSRDkb-oTBjKrHgTBj6Q5"
#include <WiFi.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>



// Thông tin WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* geoUrl = "http://ip4.iothings.vn/?geo=1"; 
const char* apiKey = "7d98331f190a3a479ab312d6e541c1d4"; 

String lat = "";
String lon = "";

ulong currentMiliseconds = 0;

bool IsReady(ulong &ulTimer, uint32_t milisecond) {
    if (currentMiliseconds - ulTimer < milisecond) return false;
    ulTimer = currentMiliseconds;
    return true;
}

void fetchData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(geoUrl);
        int httpResponseCode = http.GET();
        
        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("Dữ liệu vị trí nhận được:");
            Serial.println(payload);
            String data[7];
            int index = 0, start = 0;
            for (int i = 0; i < payload.length(); i++) {
                if (payload[i] == '|' || i == payload.length() - 1) {
                    data[index] = payload.substring(start, i + (i == payload.length() - 1 ? 1 : 0));
                    start = i + 1;
                    index++;
                    if (index >= 7) break;
                }
            }

            lat = data[6];
            lon = data[5];

            String googleMapsUrl = "https://www.google.com/maps?q=" + lat + "," + lon;

            Serial.println("Gửi dữ liệu lên Blynk...");
            Serial.print("IPv4: "); Serial.println(data[0]);
            Serial.print("Google Maps: "); Serial.println(googleMapsUrl);

            if (Blynk.connected()) {
                Blynk.virtualWrite(V1, data[0]); 
                Blynk.virtualWrite(V2, googleMapsUrl);  
            } else {
                Serial.println("Lỗi: Blynk bị mất kết nối!");
            }
        } else {
            Serial.print("Lỗi HTTP: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("Mất kết nối WiFi!");
    }
}

void fetchWeather() {
    if (lat == "" || lon == "") {
        Serial.println("Không có tọa độ, bỏ qua fetchWeather!");
        return;
    }

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String weatherUrl = "https://api.openweathermap.org/data/2.5/weather?lat=" + lat + "&lon=" + lon + "&appid=" + apiKey;


        Serial.print("Đang lấy dữ liệu thời tiết từ: ");
        Serial.println(weatherUrl);

        http.begin(weatherUrl);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String payload = http.getString();
            Serial.println("Dữ liệu thời tiết nhận được:");
            Serial.println(payload);

            
            int tempPos = payload.indexOf("\"temp\":");
            if (tempPos != -1) {
                int start = tempPos + 7; 
                int end = payload.indexOf(",", start);
                String temperature = payload.substring(start, end);

                Serial.print("Nhiệt độ: ");
                Serial.println(temperature + "°C");

                if (Blynk.connected()) {
                    Blynk.virtualWrite(V3, temperature);
                }
            } else {
                Serial.println("Không tìm thấy nhiệt độ trong JSON!");
            }
        } else {
            Serial.print("Lỗi HTTP: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("Mất kết nối WiFi!");
    }
}
void uptimeBlynk() {
    static ulong lastTime = 0;
    if (!IsReady(lastTime, 1000)) return; // Cập nhật mỗi 1 giây
    ulong value = lastTime / 1000;
    Blynk.virtualWrite(V0, value);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Đang kết nối WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n Kết nối WiFi thành công!");

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);  // Kết nối với Blynk

    fetchData();  
    fetchWeather(); 
}

void loop() {  
    currentMiliseconds = millis();
    Blynk.run();  
    uptimeBlynk();

    static ulong lastFetch = 0;
    if (IsReady(lastFetch, 30000)) { 
        fetchData();
        fetchWeather();
    }

    delay(10);  
}
