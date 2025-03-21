#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>
#include <string.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
 
String weatherApiKey = "42e8f26cbd9a92247deb6951fa50045b";
 
String city = "Hue";
 
//http config
String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=Hue&appid=42e8f26cbd9a92247deb6951fa50045b";
 
int temp = 30;
int humi = 80;
 
String httpGETRequest(const char* Url);

void setup() {
    Serial.begin(9600);
   
    // Setup wifi với timeout
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    
    int timeout = 0;
    while(WiFi.status() != WL_CONNECTED && timeout < 20) { // Timeout sau 10 giây
      delay(500);
      Serial.print(".");
      timeout++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.print("Connected to WiFi network with IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("");
      Serial.println("Cannot connect to WiFi. Check your settings.");
    }
  }
 
  void loop() {
    if (WiFi.status() == WL_CONNECTED) {
      // Read temp from OpenWeatherMap
      Serial.println(weatherUrl);
      String weatherJsonBuffer = httpGETRequest(weatherUrl.c_str());
      Serial.println(weatherJsonBuffer);
      
      if (weatherJsonBuffer != "{}") {
        JSONVar weatherJson = JSON.parse(weatherJsonBuffer);
        
        if (JSON.typeof(weatherJson) != "undefined") {
          Serial.println();
          temp = weatherJson["main"]["temp"];
          temp = temp - 273;
          Serial.print("Temperature: ");
          Serial.println(temp);
       
          humi = weatherJson["main"]["humidity"];
          Serial.print("Humidity: ");
          Serial.println(humi);
        } else {
          Serial.println("JSON parsing failed");
        }
      }
    } else {
      Serial.println("WiFi not connected. Attempting to reconnect...");
      WiFi.begin(ssid, password);
      delay(5000);
    }
    
    delay(10000);
  }
 
String httpGETRequest(const char* Url) {
  HTTPClient http;
  http.begin(Url);
  int responseCode = http.GET();
  String responseBody = "{}";
  
  if(responseCode > 0) {
    Serial.print("responseCode:");
    Serial.println(responseCode);
    responseBody = http.getString();
  } else {
    Serial.print("Error Code: ");
    Serial.println(responseCode);
  }
  
  http.end();
  return responseBody;
}