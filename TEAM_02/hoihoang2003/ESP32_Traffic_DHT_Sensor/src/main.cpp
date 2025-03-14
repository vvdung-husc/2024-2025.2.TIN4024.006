// Cấu hình Blynk
#define BLYNK_TEMPLATE_ID "TMPL6Pqb5LTxn"
#define BLYNK_TEMPLATE_NAME "ThoiTiet"
#define BLYNK_AUTH_TOKEN "EJ09Np-So_h3eu_2VdX1qKgbt_vxJOia"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Cấu hình WiFi
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// Cấu hình DHT22
#define DHTPIN 16     // Chân kết nối DHT22
#define DHTTYPE DHT22 // Loại cảm biến DHT22
DHT dht(DHTPIN, DHTTYPE);

// Chân kết nối cho LED 7 đoạn thứ nhất (A,B,C,D,E,F,G)
const uint8_t SEG1_PINS[] = {13, 12, 14, 27, 26, 25, 33};

// Chân kết nối cho LED 7 đoạn thứ hai (A,B,C,D,E,F,G)
const uint8_t SEG2_PINS[] = {4, 0, 2, 15, 32, 18, 19};

// Chân kết nối cho nút nhấn và LED xanh dương
const uint8_t BUTTON_PIN = 21;
const uint8_t BLUE_LED_PIN = 22;

// Định nghĩa thời gian cho các chu kỳ
const uint32_t SENSOR_READ_INTERVAL = 2000;    // Chu kỳ đọc cảm biến: 2 giây
const uint32_t RUNTIME_UPDATE_INTERVAL = 1000; // Chu kỳ cập nhật thời gian chạy: 1 giây

// Các biến trạng thái
uint32_t sensorReadPreviousMillis = 0; // Thời điểm đọc cảm biến gần nhất
uint32_t runtimePreviousMillis = 0;    // Thời điểm cập nhật thời gian chạy gần nhất
float temperature = 0.0;               // Nhiệt độ hiện tại
float humidity = 0.0;                  // Độ ẩm hiện tại
bool screenEnabled = true;             // Trạng thái màn hình
bool lastButtonState = HIGH;           // Trạng thái nút nhấn cuối cùng
bool buttonState;                      // Trạng thái nút nhấn hiện tại
uint32_t lastDebounceTime = 0;         // Thời gian debounce
uint32_t debounceDelay = 50;           // Độ trễ debounce
uint32_t runtimeSeconds = 0;           // Thời gian chạy (giây)
uint32_t runtimeMinutes = 0;           // Thời gian chạy (phút)

// Mảng các bit cho LED 7 đoạn (0 để sáng, 1 để tắt)
const uint8_t PROGMEM digits[10][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // Số 0
    {1, 0, 0, 1, 1, 1, 1}, // Số 1
    {0, 0, 1, 0, 0, 1, 0}, // Số 2
    {0, 0, 0, 0, 1, 1, 0}, // Số 3
    {1, 0, 0, 1, 1, 0, 0}, // Số 4
    {0, 1, 0, 0, 1, 0, 0}, // Số 5
    {0, 1, 0, 0, 0, 0, 0}, // Số 6
    {0, 0, 0, 1, 1, 1, 1}, // Số 7
    {0, 0, 0, 0, 0, 0, 0}, // Số 8
    {0, 0, 0, 0, 1, 0, 0}  // Số 9
};

// Hiển thị số trên LED 7 đoạn
void displayNumber(uint8_t number)
{
  if (!screenEnabled)
    return;

  uint8_t digit1 = min(number / 10, 9);
  uint8_t digit2 = number % 10;

  uint8_t pattern[7];

  memcpy_P(pattern, digits[digit1], 7);
  for (uint8_t i = 0; i < 7; i++)
  {
    digitalWrite(SEG1_PINS[i], pattern[i]);
  }

  memcpy_P(pattern, digits[digit2], 7);
  for (uint8_t i = 0; i < 7; i++)
  {
    digitalWrite(SEG2_PINS[i], pattern[i]);
  }
}

// Tắt LED 7 đoạn
void turnOffDisplay()
{
  for (uint8_t i = 0; i < 7; i++)
  {
    digitalWrite(SEG1_PINS[i], HIGH);
    digitalWrite(SEG2_PINS[i], HIGH);
  }
}

// Cập nhật thời gian chạy
void updateRuntime()
{
  runtimeSeconds++;
  if (runtimeSeconds >= 60)
  {
    runtimeSeconds = 0;
    runtimeMinutes++;
    if (runtimeMinutes > 99)
    {
      runtimeMinutes = 0; // Reset khi đạt 100 phút
    }
  }

  // Hiển thị số giây trên LED 7 đoạn thay vì số phút
  displayNumber(runtimeSeconds);

  // Gửi thời gian chạy lên Blynk
  char timeString[10];
  sprintf(timeString, "%02d:%02d", runtimeMinutes, runtimeSeconds);
  Blynk.virtualWrite(V1, timeString);
}

// Đọc giá trị từ cảm biến DHT22
void readSensorData()
{
  // Đọc độ ẩm
  humidity = dht.readHumidity();
  // Đọc nhiệt độ theo Celsius
  temperature = dht.readTemperature();

  // Kiểm tra nếu đọc bị lỗi
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Lỗi đọc từ cảm biến DHT!");
    return;
  }

  // Gửi dữ liệu lên Blynk
  Blynk.virtualWrite(V3, temperature);
  Blynk.virtualWrite(V4, humidity);

  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.print(" °C, Độ ẩm: ");
  Serial.print(humidity);
  Serial.println(" %");
}

// Xử lý nút nhấn
void handleButton()
{
  // Đọc trạng thái nút nhấn
  int reading = digitalRead(BUTTON_PIN);

  // Kiểm tra nút có thay đổi không
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  // Nếu trạng thái nút ổn định
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // Nếu trạng thái nút thay đổi
    if (reading != buttonState)
    {
      buttonState = reading;

      // Nếu nút được nhấn
      if (buttonState == LOW)
      {
        screenEnabled = !screenEnabled;
        digitalWrite(BLUE_LED_PIN, !screenEnabled);

        // Cập nhật hiển thị
        if (screenEnabled)
        {
          displayNumber(runtimeSeconds);
        }
        else
        {
          turnOffDisplay();
        }

        // Gửi trạng thái lên Blynk
        Blynk.virtualWrite(V2, !screenEnabled);
      }
    }
  }

  // Lưu trạng thái nút
  lastButtonState = reading;
}

// Hàm xử lý khi nhận lệnh từ Blynk
BLYNK_WRITE(V2)
{
  // Nhận giá trị từ widget V2 (nút nhấn)
  int value = param.asInt();

  // Cập nhật trạng thái màn hình
  screenEnabled = !value;
  digitalWrite(BLUE_LED_PIN, value);

  // Cập nhật hiển thị
  if (screenEnabled)
  {
    displayNumber(runtimeSeconds);
  }
  else
  {
    turnOffDisplay();
  }
}

void setup()
{
  // Cài đặt chế độ OUTPUT cho tất cả các chân LED 7 đoạn
  for (uint8_t i = 0; i < 7; i++)
  {
    pinMode(SEG1_PINS[i], OUTPUT);
    pinMode(SEG2_PINS[i], OUTPUT);
  }

  // Cài đặt chế độ OUTPUT cho LED xanh dương
  pinMode(BLUE_LED_PIN, OUTPUT);
  digitalWrite(BLUE_LED_PIN, LOW);

  // Cài đặt chế độ INPUT_PULLUP cho nút nhấn
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonState = digitalRead(BUTTON_PIN);
  lastButtonState = buttonState;

  // Khởi tạo giao tiếp Serial
  Serial.begin(115200);
  Serial.println("Khởi động hệ thống...");

  // Khởi tạo cảm biến DHT
  dht.begin();

  // Kết nối WiFi
  Serial.print("Kết nối WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi đã kết nối");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());

  // Kết nối đến Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Đọc giá trị ban đầu từ cảm biến
  readSensorData();

  // Hiển thị số giây ban đầu (0) thay vì số phút
  displayNumber(0);
}

void loop()
{
  // Xử lý Blynk
  Blynk.run();

  // Xử lý nút nhấn
  handleButton();

  uint32_t currentMillis = millis();

  // Đọc cảm biến định kỳ
  if (currentMillis - sensorReadPreviousMillis >= SENSOR_READ_INTERVAL)
  {
    sensorReadPreviousMillis = currentMillis;
    readSensorData();
  }

  // Cập nhật thời gian chạy mỗi giây
  if (currentMillis - runtimePreviousMillis >= RUNTIME_UPDATE_INTERVAL)
  {
    runtimePreviousMillis = currentMillis;
    updateRuntime();
  }
}