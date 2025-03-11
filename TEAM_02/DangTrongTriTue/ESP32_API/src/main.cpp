#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>


// WiFi credentials
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6lD14z4JL"
#define BLYNK_TEMPLATE_NAME "ESP 32 Led controll"
#define BLYNK_AUTH_TOKEN "SsfKH68GnPaMpwFo7SuSHWogtbX80vw3"
#include <BlynkSimpleEsp32.h>
// Weather API configuration
const String weatherApiKey = "42e8f26cbd9a92247deb6951fa50045b";
const String city = "Hue";
const String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=Hue&appid=f9656dff552e598e6f9be547a9592836";

// IP-API to get IP information
const String ipUrl = "http://ip-api.com/json/";

// Variables
int temp = 30;
int humi = 80;
float longitude = 0.0;
float latitude = 0.0;
String ipAddress = "0.0.0.0";
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 60000; // Update weather every 60 seconds

// Timer for Blynk
BlynkTimer timer;

// Function prototypes
String httpGETRequest(const char *url);
void updateUptime();
void updateWeather();
void getIPAddress();

// This function is called when Blynk connects or reconnects
BLYNK_CONNECTED() {
  Serial.println("Connected to Blynk server");
  // Re-send all data when reconnected
  Blynk.virtualWrite(V0, millis() / 1000);
  Blynk.virtualWrite(V2, humi);
  Blynk.virtualWrite(V3, temp);
  Blynk.virtualWrite(V4, ipAddress);
  Blynk.virtualWrite(V5, String(latitude, 6) + ", " + String(longitude, 6));
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Starting ESP32 Weather Station");

  // Connect to WiFi with timeout
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) { // 10 second timeout
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

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  
  // Setup timers
  timer.setInterval(1000L, updateUptime);     // Update uptime every second
  timer.setInterval(60000L, updateWeather);   // Update weather every minute
  timer.setInterval(300000L, getIPAddress);   // Update IP address every 5 minutes
  
  // Initial data fetch
  getIPAddress();
  updateWeather();
}

void loop() {
  Blynk.run();  // Process Blynk commands
  timer.run();  // Process timer events
  
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    WiFi.begin(ssid, password);
    delay(5000);
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconnected to WiFi");
      getIPAddress();
      updateWeather();
    }
  }
}

// Get the external IPv4 address
void getIPAddress() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Fetching IP information...");
    
    String ipJsonBuffer = httpGETRequest(ipUrl.c_str());
    Serial.println("IP API Response: " + ipJsonBuffer);

    if (ipJsonBuffer != "{}" && ipJsonBuffer.length() > 5) {
      JSONVar ipJson = JSON.parse(ipJsonBuffer);

      if (JSON.typeof(ipJson) != "undefined") {
        // Get the external IP address
        if (ipJson.hasOwnProperty("query")) {
          ipAddress = (const char*)ipJson["query"];
          Serial.print("External IP Address: ");
          Serial.println(ipAddress);
          
          // Send to Blynk
          Serial.println("Sending IP address to Blynk V4: " + ipAddress);
          Blynk.virtualWrite(V4, ipAddress);
        } else {
          Serial.println("Error: 'query' property not found in IP JSON");
        }
      } else {
        Serial.println("IP JSON parsing failed");
      }
    } else {
      Serial.println("Received empty or invalid response from IP API");
    }
  } else {
    Serial.println("Cannot fetch IP: WiFi not connected");
  }
}

// Update and send device uptime to Blynk
void updateUptime() {
  int uptime = millis() / 1000;
  
  Blynk.virtualWrite(V0, uptime);
  // Serial.print("Thời gian hoạt động: ");
  // Serial.print(uptime / 60);
  // Serial.print(" phút ");
  // Serial.print(uptime % 60);
  // Serial.println(" giây");
}

// Update weather data from API and send to Blynk
void updateWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    // Read temperature and humidity from OpenWeatherMap
    Serial.println("Fetching weather data...");
    
    String weatherJsonBuffer = httpGETRequest(weatherUrl.c_str());
    Serial.println("Weather API Response: " + weatherJsonBuffer);

    if (weatherJsonBuffer != "{}" && weatherJsonBuffer.length() > 5) {
      JSONVar weatherJson = JSON.parse(weatherJsonBuffer);

      if (JSON.typeof(weatherJson) != "undefined") {
        // Parse temperature (convert from Kelvin to Celsius)
        if (weatherJson.hasOwnProperty("main") && weatherJson["main"].hasOwnProperty("temp")) {
          float kelvin = (double)weatherJson["main"]["temp"];
          temp = kelvin - 273.15;  // Convert directly from Kelvin to Celsius
          Serial.print("Temperature: ");
          Serial.println(temp);
        } else {
          Serial.println("Error: Temperature data not found in weather JSON");
        }

        // Parse humidity
        if (weatherJson.hasOwnProperty("main") && weatherJson["main"].hasOwnProperty("humidity")) {
          humi = (int)weatherJson["main"]["humidity"];
          Serial.print("Humidity: ");
          Serial.println(humi);
        } else {
          Serial.println("Error: Humidity data not found in weather JSON");
        }
        
        // Parse longitude and latitude
        if (weatherJson.hasOwnProperty("coord")) {
          if (weatherJson["coord"].hasOwnProperty("lon")) {
            longitude = (double)weatherJson["coord"]["lon"];
            Serial.print("Longitude: ");
            Serial.println(longitude, 6);
          }
          
          if (weatherJson["coord"].hasOwnProperty("lat")) {
            latitude = (double)weatherJson["coord"]["lat"];
            Serial.print("Latitude: ");
            Serial.println(latitude, 6);
          }
        } else {
          Serial.println("Error: Coordinate data not found in weather JSON");
        }
        
        // Create coordinate string
        String coordinates = String(latitude, 6) + ", " + String(longitude, 6);
        
        // Send to Blynk with debugging info
        Serial.println("Sending data to Blynk:");
        Serial.println("Temperature (V3): " + String(temp));
        Serial.println("Humidity (V2): " + String(humi));
        Serial.println("Coordinates (V5): " + coordinates);
        
        Blynk.virtualWrite(V3, temp);
        Blynk.virtualWrite(V2, humi);
        Blynk.virtualWrite(V5, coordinates);
        
        Serial.println("Data sent to Blynk successfully");
      } else {
        Serial.println("Weather JSON parsing failed");
      }
    } else {
      Serial.println("Received empty or invalid response from Weather API");
    }
  } else {
    Serial.println("Cannot fetch weather: WiFi not connected");
  }
}

// HTTP GET request function
String httpGETRequest(const char *url) {
  HTTPClient http;
  http.begin(url);
  int responseCode = http.GET();
  String responseBody = "{}";

  if (responseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(responseCode);
    responseBody = http.getString();
  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(responseCode);
  }

  http.end();
  return responseBody;
}