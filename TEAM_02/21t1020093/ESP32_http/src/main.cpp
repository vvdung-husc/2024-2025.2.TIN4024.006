#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>


#define BLYNK_TEMPLATE_ID "TMPL68Mx5ai8z"
#define BLYNK_TEMPLATE_NAME "ESP32HTTP"
#define BLYNK_AUTH_TOKEN "8oOduSUCNu7Zsth3D3ylUjVkfQfsT7Ug"
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";  // Tên WiFi
char pass[] = "";             // Mật khẩu WiFi
const char* geoApi = "http://ip4.iothings.vn?geo=1";
const char* weatherKey = "c580c6bf176ca20d5c77ef5547b028aa";

unsigned long lastTime = 0;
const long delayTime = 1000;
void connectWiFi();
void updateUptime();
void fetchGeoData();
void fetchWeather(float lat, float lon);
String getDMS(float value, char positive, char negative);
void parseGeoData(String data, String parts[]);

void setup() {
    Serial.begin(115200);
    connectWiFi();
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    fetchGeoData();
}

void loop() {
    Blynk.run();
    updateUptime();
}

void connectWiFi() {
    WiFi.begin(ssid, pass);
    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
}

void updateUptime() {
    if (millis() - lastTime >= delayTime) {
        int uptime = millis() / 1000;
        Serial.println("Uptime: " + String(uptime));
        Blynk.virtualWrite(V0, uptime);
        lastTime = millis();
    }
}

String getDMS(float value, char positive, char negative) {
    char buffer[20];
    char dir = (value >= 0) ? positive : negative;
    value = abs(value);
    int deg = (int)value;
    float min = (value - deg) * 60;
    int wholeMin = (int)min;
    float sec = (min - wholeMin) * 60;
    snprintf(buffer, 20, "%d°%d'%.1f\"%c", deg, wholeMin, sec, dir);
    return String(buffer);
}

void parseGeoData(String data, String parts[]) {
    int pos = 0, idx = 0;
    while (idx < 7) {
        int next = data.indexOf('|', pos);
        if (next == -1) {
            parts[idx++] = data.substring(pos);
            break;
        }
        parts[idx++] = data.substring(pos, next);
        pos = next + 1;
    }
}

void fetchGeoData() {
    HTTPClient http;
    http.begin(geoApi);
    int code = http.GET();

    if (code > 0) {
        String response = http.getString();
        Serial.println("Geo API Response: " + response);

        String parts[7];
        parseGeoData(response, parts);

        if (parts[6] != "") {
            float lat = parts[6].toFloat();
            float lon = parts[5].toFloat();

            String latDMS = getDMS(lat, 'N', 'S');
            String lonDMS = getDMS(lon, 'E', 'W');

            Serial.println("=== IP Info ===");
            Serial.println("IP: " + parts[0]);
            Serial.println("Country Code: " + parts[1]);
            Serial.println("Country: " + parts[2]);
            Serial.println("Region: " + parts[3]);
            Serial.println("City: " + parts[4]);
            Serial.println("Lat: " + parts[6]);
            Serial.println("Lon: " + parts[5]);
            Serial.println("Coords (DMS): " + latDMS + ", " + lonDMS);

            Blynk.virtualWrite(V1, parts[0]);
            String mapsLink = "http://www.google.com/maps/place/" + parts[6] + "," + parts[5];
            Serial.println("Google Maps: " + mapsLink);
            Blynk.virtualWrite(V2, mapsLink);

            fetchWeather(lat, lon);
        } else {
            Serial.println("Invalid response format!");
        }
    } else {
        Serial.println("Geo API Error: " + String(code));
    }
    http.end();
}

void fetchWeather(float lat, float lon) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(lat) +
                 "&lon=" + String(lon) + "&units=metric&appid=" + weatherKey;
    http.begin(client, url);

    int code = http.GET();
    if (code > 0) {
        String json = http.getString();
        Serial.println("Weather Data: " + json);

        StaticJsonDocument<1024> doc;
        if (!deserializeJson(doc, json)) {
            float temp = doc["main"]["temp"];
            Serial.println("Temperature: " + String(temp) + "°C");
            Blynk.virtualWrite(V3, temp);
        } else {
            Serial.println("JSON parsing failed!");
        }
    } else {
        Serial.println("Weather API Error: " + String(code));
    }
    http.end();
}