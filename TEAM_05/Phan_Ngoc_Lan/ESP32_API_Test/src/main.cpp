#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "Wokwi-GUEST";
const char *password = "";
const char *serverUrl = "http://ip4.iothings.vn/?geo=1";

// Khai báo trước hàm getGeoInfo()
void getGeoInfo();

void connectToWiFi()
{
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, IPAddress(8, 8, 8, 8)); // Cấu hình DNS thủ công
  while (WiFi.status() != WL_CONNECTED)
  {
    yield();
  }
  Serial.println("\nConnected WiFi");
}

void setup()
{
  Serial.begin(115200);
  connectToWiFi();
  getGeoInfo(); // Gọi sau khi đã khai báo trước
}

void loop()
{
  // Không cần lặp lại liên tục, chỉ lấy thông tin một lần
}

void getGeoInfo()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Không thể kết nối đến WiFi, retrying...");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK)
  {
    String payload = http.getString();
    Serial.println("Response: " + payload);

    int firstSep = payload.indexOf('|');
    int lastSep = payload.lastIndexOf('|');
    int secondLastSep = payload.lastIndexOf('|', lastSep - 1);

    String ip = payload.substring(0, firstSep);
    String longitude = payload.substring(secondLastSep + 1, lastSep);
    String latitude = payload.substring(lastSep + 1);

    Serial.println("IPv4: " + ip);
    Serial.println("Latitude: " + latitude);
    Serial.println("Longitude: " + longitude);

    String googleMapsLink = "https://www.google.com/maps?q=" + latitude + "," + longitude;
    Serial.println("Google Maps Link: " + googleMapsLink);
  }
  else
  {
    Serial.printf("HTTP Request failed, error: %d\n", httpCode);
  }
  http.end();
}