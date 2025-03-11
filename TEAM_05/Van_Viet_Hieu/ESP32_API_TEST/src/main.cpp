#include <WiFi.h>
#include <HTTPClient.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";
const char* api_url = "http://ip-api.com/json";

void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    Serial.println("\nConnected to WiFi");
}

String getLocationData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(api_url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            return http.getString();
        } else {
            return "Error on HTTP request";
        }
        http.end();
    }
    return "WiFi not connected";
}

void extractAndPrintLocation(String payload) {
    int ipIndex = payload.indexOf("\"query\":\"") + 9;
    int latIndex = payload.indexOf("\"lat\":") + 6;
    int lonIndex = payload.indexOf("\"lon\":") + 6;
    
    String ip = payload.substring(ipIndex, payload.indexOf("\"", ipIndex));
    String lat = payload.substring(latIndex, payload.indexOf(",", latIndex));
    String lon = payload.substring(lonIndex, payload.indexOf(",", lonIndex));
    
    Serial.println("IPv4 Address: " + ip);
    Serial.println("Latitude: " + lat);
    Serial.println("Longitude: " + lon);
    Serial.println("Google Maps Link: https://www.google.com/maps?q=" + lat + "," + lon);
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    String locationData = getLocationData();
    Serial.println("Response:");
    Serial.println(locationData);
    extractAndPrintLocation(locationData);
}

void loop() {
    vTaskDelay(10000 / portTICK_PERIOD_MS); // Đợi 10 giây trước khi gửi lại yêu cầu
}
