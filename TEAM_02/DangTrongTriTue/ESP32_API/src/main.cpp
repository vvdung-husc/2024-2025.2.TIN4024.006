#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>

// Thông tin WiFi
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// Thông tin Blynk
#define BLYNK_TEMPLATE_ID "TMPL6lD14z4JL"
#define BLYNK_TEMPLATE_NAME "ESP 32 Led controll"
#define BLYNK_AUTH_TOKEN "SsfKH68GnPaMpwFo7SuSHWogtbX80vw3"
#include <BlynkSimpleEsp32.h>

// Cấu hình API thời tiết
const String weatherApiKey = "42e8f26cbd9a92247deb6951fa50045b";
const String city = "Hue";
const String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=Hue&appid=f9656dff552e598e6f9be547a9592836";

// IP-API để lấy thông tin IP
const String ipUrl = "http://ip-api.com/json/";

// Biến
int temp = 30;
int humi = 80;
float longitude = 0.0;
float latitude = 0.0;
String ipAddress = "0.0.0.0";
String countryCode = "";
String country = "";
String region = "";
String city_name = "";
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 60000; // Cập nhật thời tiết mỗi 60 giây

// Timer cho Blynk
BlynkTimer timer;

// Khai báo hàm
String httpGETRequest(const char *url);
void updateUptime();
void updateWeather();
void getIPAddress();

// Hàm này được gọi khi Blynk kết nối hoặc kết nối lại
BLYNK_CONNECTED()
{
  Serial.println("Đã kết nối tới server Blynk");
  // Gửi lại tất cả dữ liệu khi kết nối lại
  Blynk.virtualWrite(V0, millis() / 1000);
  Blynk.virtualWrite(V2, humi);
  Blynk.virtualWrite(V3, temp);

  // Gửi IP với định dạng tùy chỉnh đến V4
  String ipInfoString = ipAddress + "|" + countryCode + "|" + country + "|" + region + "|" + city_name + "|" + String(longitude, 6) + "|" + String(latitude, 6);
  Blynk.virtualWrite(V4, ipInfoString);

  // Gửi link Google Maps đến V5
  String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
  Blynk.virtualWrite(V5, googleMapsLink);
}

void setup()
{
  // Khởi tạo kết nối serial
  Serial.begin(9600);
  Serial.println("Đang khởi động trạm thời tiết ESP32");

  // Kết nối WiFi với timeout
  WiFi.begin(ssid, password);
  Serial.println("Đang kết nối WiFi...");

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20)
  { // timeout 10 giây
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("Đã kết nối tới mạng WiFi với địa chỉ IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("Không thể kết nối WiFi. Kiểm tra lại cài đặt.");
  }

  // Khởi tạo Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Cài đặt timers
  timer.setInterval(1000L, updateUptime);   // Cập nhật thời gian hoạt động mỗi giây
  timer.setInterval(60000L, updateWeather); // Cập nhật thời tiết mỗi phút
  timer.setInterval(300000L, getIPAddress); // Cập nhật địa chỉ IP mỗi 5 phút

  // Lấy dữ liệu ban đầu
  getIPAddress();
  updateWeather();
}

void loop()
{
  Blynk.run(); // Xử lý lệnh Blynk
  timer.run(); // Xử lý sự kiện timer

  // Kết nối lại WiFi nếu bị ngắt kết nối
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi không được kết nối. Đang thử kết nối lại...");
    WiFi.begin(ssid, password);
    delay(5000);

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Đã kết nối lại WiFi");
      getIPAddress();
      updateWeather();
    }
  }
}

// Lấy địa chỉ IPv4 bên ngoài và vị trí địa lý
void getIPAddress()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Đang lấy thông tin IP...");

    String ipJsonBuffer = httpGETRequest(ipUrl.c_str());
    Serial.println("Phản hồi IP API: " + ipJsonBuffer);

    if (ipJsonBuffer != "{}" && ipJsonBuffer.length() > 5)
    {
      JSONVar ipJson = JSON.parse(ipJsonBuffer);

      if (JSON.typeof(ipJson) != "undefined")
      {
        // Lấy thông tin từ IP API
        if (ipJson.hasOwnProperty("query"))
        {
          ipAddress = (const char *)ipJson["query"];
          Serial.print("Địa chỉ IP ngoài: ");
          Serial.println(ipAddress);
        }

        if (ipJson.hasOwnProperty("countryCode"))
        {
          countryCode = (const char *)ipJson["countryCode"];
        }
        else
        {
          countryCode = "Unknown";
        }

        if (ipJson.hasOwnProperty("country"))
        {
          country = (const char *)ipJson["country"];
        }
        else
        {
          country = "Unknown";
        }

        if (ipJson.hasOwnProperty("regionName"))
        {
          region = (const char *)ipJson["regionName"];
        }
        else
        {
          region = "Unknown";
        }

        if (ipJson.hasOwnProperty("city"))
        {
          city_name = (const char *)ipJson["city"];
        }
        else
        {
          city_name = "Unknown";
        }

        if (ipJson.hasOwnProperty("lon"))
        {
          longitude = (double)ipJson["lon"];
        }

        if (ipJson.hasOwnProperty("lat"))
        {
          latitude = (double)ipJson["lat"];
        }

        // Tạo chuỗi thông tin IP theo định dạng yêu cầu
        String ipInfoString = ipAddress + "|" + countryCode + "|" + country + "|" + region + "|" + city_name + "|" + String(longitude, 6) + "|" + String(latitude, 6);

        Serial.println("Thông tin IP đầy đủ:");
        Serial.println(ipInfoString);

        // Gửi thông tin IP và vị trí IP lên Blynk
        Serial.println("Gửi thông tin IP lên Blynk V4");
        Blynk.virtualWrite(V4, ipInfoString);

        // Tạo và gửi link Google Maps cho vị trí IP lên V5
        String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
        Serial.println("Gửi link Google Maps lên Blynk V5: " + googleMapsLink);
        Blynk.virtualWrite(V5, googleMapsLink);
      }
      else
      {
        Serial.println("Phân tích JSON IP thất bại");
      }
    }
    else
    {
      Serial.println("Nhận phản hồi trống hoặc không hợp lệ từ IP API");
    }
  }
  else
  {
    Serial.println("Không thể lấy IP: WiFi không được kết nối");
  }
}

// Cập nhật và gửi thời gian hoạt động của thiết bị lên Blynk
void updateUptime()
{
  int uptime = millis() / 1000;

  Blynk.virtualWrite(V0, uptime);
  // Serial.print("Thời gian hoạt động: ");
  // Serial.print(uptime / 60);
  // Serial.print(" phút ");
  // Serial.print(uptime % 60);
  // Serial.println(" giây");
}

// Cập nhật dữ liệu thời tiết từ API và gửi lên Blynk
void updateWeather()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    // Đọc nhiệt độ và độ ẩm từ OpenWeatherMap
    Serial.println("Đang lấy dữ liệu thời tiết...");

    String weatherJsonBuffer = httpGETRequest(weatherUrl.c_str());
    Serial.println("Phản hồi API thời tiết: " + weatherJsonBuffer);

    if (weatherJsonBuffer != "{}" && weatherJsonBuffer.length() > 5)
    {
      JSONVar weatherJson = JSON.parse(weatherJsonBuffer);

      if (JSON.typeof(weatherJson) != "undefined")
      {
        // Phân tích nhiệt độ (chuyển từ Kelvin sang Celsius)
        if (weatherJson.hasOwnProperty("main") && weatherJson["main"].hasOwnProperty("temp"))
        {
          float kelvin = (double)weatherJson["main"]["temp"];
          temp = kelvin - 273.15; // Chuyển trực tiếp từ Kelvin sang Celsius
          Serial.print("Nhiệt độ: ");
          Serial.println(temp);
        }
        else
        {
          Serial.println("Lỗi: Không tìm thấy dữ liệu nhiệt độ trong JSON thời tiết");
        }

        // Phân tích độ ẩm
        if (weatherJson.hasOwnProperty("main") && weatherJson["main"].hasOwnProperty("humidity"))
        {
          humi = (int)weatherJson["main"]["humidity"];
          Serial.print("Độ ẩm: ");
          Serial.println(humi);
        }
        else
        {
          Serial.println("Lỗi: Không tìm thấy dữ liệu độ ẩm trong JSON thời tiết");
        }

        // Phân tích kinh độ và vĩ độ
        if (weatherJson.hasOwnProperty("coord"))
        {
          if (weatherJson["coord"].hasOwnProperty("lon"))
          {
            longitude = (double)weatherJson["coord"]["lon"];
            Serial.print("Kinh độ: ");
            Serial.println(longitude, 6);
          }

          if (weatherJson["coord"].hasOwnProperty("lat"))
          {
            latitude = (double)weatherJson["coord"]["lat"];
            Serial.print("Vĩ độ: ");
            Serial.println(latitude, 6);
          }
        }
        else
        {
          Serial.println("Lỗi: Không tìm thấy dữ liệu tọa độ trong JSON thời tiết");
        }

        // Gửi nhiệt độ và độ ẩm lên Blynk
        Serial.println("Gửi dữ liệu thời tiết lên Blynk:");
        Serial.println("Nhiệt độ (V3): " + String(temp));
        Serial.println("Độ ẩm (V2): " + String(humi));

        Blynk.virtualWrite(V3, temp);
        Blynk.virtualWrite(V2, humi);

        // Tạo và gửi link Google Maps cho vị trí thời tiết lên V5
        String googleMapsLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
        Serial.println("Gửi link Google Maps lên Blynk V5: " + googleMapsLink);
        Blynk.virtualWrite(V5, googleMapsLink);

        Serial.println("Dữ liệu đã được gửi lên Blynk thành công");
      }
      else
      {
        Serial.println("Phân tích JSON thời tiết thất bại");
      }
    }
    else
    {
      Serial.println("Nhận phản hồi trống hoặc không hợp lệ từ API thời tiết");
    }
  }
  else
  {
    Serial.println("Không thể lấy thời tiết: WiFi không được kết nối");
  }
}

// Hàm HTTP GET request
String httpGETRequest(const char *url)
{
  HTTPClient http;
  http.begin(url);
  int responseCode = http.GET();
  String responseBody = "{}";

  if (responseCode > 0)
  {
    Serial.print("Mã phản hồi HTTP: ");
    Serial.println(responseCode);
    responseBody = http.getString();
  }
  else
  {
    Serial.print("Mã lỗi HTTP: ");
    Serial.println(responseCode);
  }

  http.end();
  return responseBody;
}