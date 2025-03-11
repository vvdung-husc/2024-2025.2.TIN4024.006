#include <Arduino.h>

#include <DHT.h>

// Định nghĩa Blynk
#define BLYNK_TEMPLATE_ID "TMPL6lD14z4JL"
#define BLYNK_TEMPLATE_NAME "ESP 32 Led controll"
#define BLYNK_AUTH_TOKEN "SsfKH68GnPaMpwFo7SuSHWogtbX80vw3"
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
// Thông tin WiFi - Hãy thay thế bằng thông tin WiFi của bạn
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN 35
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// Định nghĩa chân kết nối cho đèn giao thông
const uint8_t RED_PIN = 16, YELLOW_PIN = 17, GREEN_PIN = 5;
const uint8_t BLUE_PIN = 22;         // Chân cho đèn LED xanh dương
const uint8_t BUTTON_PIN = 21;       // Chân cho nút nhấn
const uint8_t LIGHT_SENSOR_PIN = 23; // Chân cho cảm biến ánh sáng

// Chân kết nối cho LED 7 đoạn thứ nhất (A,B,C,D,E,F,G)
const uint8_t SEG1_PINS[] = {13, 12, 14, 27, 26, 25, 33};

// Chân kết nối cho LED 7 đoạn thứ hai (A,B,C,D,E,F,G)
const uint8_t SEG2_PINS[] = {4, 0, 2, 15, 32, 18, 19};

// Các trạng thái của đèn giao thông
enum TrafficState : uint8_t
{
  RED_STATE,   // Trạng thái đèn đỏ
  GREEN_STATE, // Trạng thái đèn xanh
  YELLOW_STATE // Trạng thái đèn vàng
};

// Định nghĩa thời gian cho mỗi trạng thái (đơn vị milli giây)
uint32_t RED_DURATION = 20000;            // Thời gian đèn đỏ: 20 giây
uint32_t GREEN_DURATION = 10000;          // Thời gian đèn xanh: 10 giây
uint32_t YELLOW_DURATION = 3000;          // Thời gian đèn vàng: 3 giây
const uint32_t COUNTDOWN_INTERVAL = 1000; // Chu kỳ đếm ngược: 1 giây
const uint32_t BLINK_INTERVAL = 500;      // Chu kỳ nhấp nháy đèn vàng: 0.5 giây

// Các biến trạng thái
TrafficState currentState = RED_STATE; // Trạng thái hiện tại
uint32_t stateStartMillis = 0;         // Thời điểm bắt đầu trạng thái
uint32_t countdownPreviousMillis = 0;  // Thời điểm đếm ngược gần nhất
uint32_t blinkPreviousMillis = 0;      // Thời điểm nhấp nháy gần nhất
uint8_t countdown = 0;                 // Giá trị đếm ngược hiện tại
bool displayOn = true;                 // Trạng thái hiển thị LED 7 đoạn
bool displayUserChoice = true;         // Trạng thái người dùng muốn màn hình LED
bool blueLedOn = true;                 // Trạng thái đèn LED xanh dương
bool buttonState = HIGH;               // Trạng thái nút nhấn (HIGH khi không nhấn)
bool lastButtonState = HIGH;           // Trạng thái nút nhấn trước đó
uint32_t lastDebounceTime = 0;         // Thời điểm debounce nút nhấn gần nhất
const uint32_t debounceDelay = 20;     // Thời gian debounce: 20ms
bool isNightMode = false;              // Chế độ đêm (khi trời tối)
bool yellowBlinkState = false;         // Trạng thái nhấp nháy đèn vàng
bool manualMode = false;               // Chế độ điều khiển thủ công qua Blynk

// Timer để cập nhật Blynk
BlynkTimer timer;

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

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Lỗi đọc cảm biến DHT22!");
    return;
  }
  Serial.print("Nhiệt độ: ");
  Serial.print(t);
  Serial.println("°C");
  Serial.print("Độ ẩm: ");
  Serial.print(h);
  Serial.println("%");
  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V2, h);
}
// Cập nhật trạng thái đèn giao thông lên Blynk
// void updateBlynkLights()
// {
//   Gửi trạng thái đèn giao thông hiện tại đến Blynk
//   Sử dụng các kênh ảo V0, V1, V2 cho đèn đỏ, xanh, vàng
//   Blynk.virtualWrite(V0, currentState == RED_STATE ? 1 : 0);
//   Blynk.virtualWrite(V1, currentState == GREEN_STATE ? 1 : 0);
//   Blynk.virtualWrite(V2, currentState == YELLOW_STATE ? 1 : 0);

//   Gửi thời gian đếm ngược
//   Blynk.virtualWrite(V3, countdown);

//   Gửi thông tin về chế độ ban đêm
//   Blynk.virtualWrite(V5, isNightMode ? 1 : 0);
// }
// Hàm để lấy tên trạng thái dưới dạng String
String getStateName(TrafficState state)
{
  switch (state)
  {
  case RED_STATE:
    return "ĐỎ";
  case GREEN_STATE:
    return "XANH";
  case YELLOW_STATE:
    return "VÀNG";
  default:
    return "KHÔNG XÁC ĐỊNH";
  }
}

// Hàm để lấy thời gian của trạng thái (đơn vị giây)
uint8_t getStateDuration(TrafficState state)
{
  switch (state)
  {
  case RED_STATE:
    return RED_DURATION / 1000;
  case GREEN_STATE:
    return GREEN_DURATION / 1000;
  case YELLOW_STATE:
    return YELLOW_DURATION / 1000;
  default:
    return 0;
  }
}

void turnOffDisplay()
{
  // Tắt tất cả các phân đoạn bằng cách đặt chúng thành HIGH (vì mạch dùng logic đảo)
  for (uint8_t i = 0; i < 7; i++)
  {
    digitalWrite(SEG1_PINS[i], HIGH);
    digitalWrite(SEG2_PINS[i], HIGH);
  }
}

void displayNumber(uint8_t number)
{
  if (!displayOn)
  {
    turnOffDisplay();
    return;
  }

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

// Hàm xử lý hiển thị LED 7 đoạn
void updateDisplayState()
{
  // Cập nhật trạng thái hiển thị dựa trên lựa chọn người dùng và chế độ ban đêm
  // Nếu chế độ ban đêm, màn hình sẽ luôn tắt bất kể lựa chọn của người dùng
  displayOn = displayUserChoice && !isNightMode;

  // Cập nhật hiển thị
  if (displayOn)
  {
    displayNumber(countdown);
  }
  else
  {
    turnOffDisplay();
  }

  // Cập nhật đèn xanh dương - bật khi màn hình bật, tắt khi màn hình tắt
  digitalWrite(BLUE_PIN, displayOn ? HIGH : LOW);

  // Gửi trạng thái hiện tại đến Blynk
  Blynk.virtualWrite(V1, displayUserChoice ? 1 : 0);
}
void changeState(TrafficState newState)
{
  // Lấy tên trạng thái hiện tại và mới
  String currentStateName = getStateName(currentState);
  String newStateName = getStateName(newState);

  // In thông báo chuyển trạng thái
  Serial.print("Chuyển đèn: ");
  Serial.print(currentStateName);
  Serial.print(" -> ");
  Serial.print(newStateName);
  Serial.print(" (");
  Serial.print(getStateDuration(newState));
  Serial.println(" giây)");

  // Tắt tất cả đèn
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  // Bật đèn mới và cập nhật trạng thái
  if (!isNightMode)
  {
    switch (newState)
    {
    case RED_STATE:
      digitalWrite(RED_PIN, HIGH);
      countdown = RED_DURATION / 1000 - 1;
      break;
    case GREEN_STATE:
      digitalWrite(GREEN_PIN, HIGH);
      countdown = GREEN_DURATION / 1000 - 1;
      break;
    case YELLOW_STATE:
      digitalWrite(YELLOW_PIN, HIGH);
      countdown = YELLOW_DURATION / 1000 - 1;
      break;
    }
  }

  currentState = newState;
  stateStartMillis = millis();
  updateDisplayState();

  // Cập nhật trạng thái đèn trên Blynk
  // updateBlynkLights();
}

void handleButton()
{
  // Đọc trạng thái nút nhấn
  int reading = digitalRead(BUTTON_PIN);

  // Kiểm tra nếu trạng thái nút nhấn đã thay đổi
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  // Kiểm tra trạng thái nút sau thời gian debounce
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // Nếu trạng thái đã thay đổi sau debounce
    if (reading != buttonState)
    {
      buttonState = reading;

      // Nếu nút được nhấn (LOW với INPUT_PULLUP)
      if (buttonState == LOW)
      {
        // Đảo trạng thái hiển thị ngay lập tức
        displayUserChoice = !displayUserChoice;

        // Cập nhật LED ngay lập tức không đợi hàm updateDisplayState
        digitalWrite(BLUE_PIN, displayUserChoice ? HIGH : LOW);

        // Sau đó mới cập nhật các thành phần khác
        updateDisplayState();

        Serial.print("Màn hình được chuyển bởi nút nhấn: ");
        Serial.println(displayUserChoice ? "BẬT" : "TẮT");
      }
    }
  }

  // Lưu trạng thái nút nhấn hiện tại
  lastButtonState = reading;
}
void handleLightSensor()
{
  // Đọc giá trị từ cảm biến ánh sáng (LOW khi trời tối, HIGH khi trời sáng)
  bool isDark = digitalRead(LIGHT_SENSOR_PIN) == HIGH;

  // Nếu có sự thay đổi trạng thái ánh sáng
  if (isDark != isNightMode)
  {
    isNightMode = isDark;

    if (isNightMode)
    {
      // Chuyển sang chế độ đêm - tắt đèn giao thông bình thường và tắt màn hình LED
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      // Đèn vàng sẽ được xử lý nhấp nháy trong loop()

      // Tắt màn hình LED khi chuyển sang chế độ đêm
      updateDisplayState();

      Serial.println("Night mode activated - Display forced OFF");
    }
    else
    {
      // Quay lại chế độ bình thường - khởi động lại trạng thái hiện tại
      yellowBlinkState = false;
      digitalWrite(YELLOW_PIN, LOW);

      // Cập nhật trạng thái hiển thị dựa trên lựa chọn người dùng
      updateDisplayState();

      // Khởi động lại trạng thái hiện tại
      changeState(currentState);

      Serial.println("Day mode activated - Display restored to user choice");
    }
  }
}

void handleNightMode()
{
  uint32_t currentMillis = millis();

  // Xử lý nhấp nháy đèn vàng khi ở chế độ đêm
  if (isNightMode && (currentMillis - blinkPreviousMillis >= BLINK_INTERVAL))
  {
    blinkPreviousMillis = currentMillis;
    yellowBlinkState = !yellowBlinkState;
    digitalWrite(YELLOW_PIN, yellowBlinkState ? HIGH : LOW);
  }
}
// Trong hàm sendSensorData(), cập nhật trạng thái màn hình lên Blynk
void sendSensorData()
{
  // Cập nhật trạng thái đèn
  // updateBlynkLights();

  // Cập nhật trạng thái màn hình
  Blynk.virtualWrite(V1, displayUserChoice ? 1 : 0);
}
// Điều khiển bật/tắt màn hình từ Blynk - sử dụng V4
BLYNK_WRITE(V1)
{
  // Lấy giá trị từ Blynk (0 hoặc 1)
  displayUserChoice = param.asInt();

  // Cập nhật trạng thái hiển thị
  updateDisplayState();

  Serial.print("Màn hình được chuyển bởi Blynk: ");
  Serial.println(displayUserChoice ? "BẬT" : "TẮT");
}

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  dht.begin();
  // Kết nối đến Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // Cài đặt chế độ OUTPUT cho tất cả các chân
  for (uint8_t i = 0; i < 7; i++)
  {
    pinMode(SEG1_PINS[i], OUTPUT);
    pinMode(SEG2_PINS[i], OUTPUT);
  }

  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Cài đặt chế độ INPUT cho nút nhấn và cảm biến ánh sáng
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  // Bật đèn LED xanh dương ban đầu
  digitalWrite(BLUE_PIN, HIGH);

  // Khởi tạo Serial để debug

  Serial.println("Traffic Light System Starting with Blynk...");
  Blynk.virtualWrite(V2, dht.readHumidity());
  Blynk.virtualWrite(V3, dht.readTemperature());
  // Thiết lập đọc độ ẩm, nhiệt độ gửi lên mỗi lần 2s
  timer.setInterval(2000L, sendSensor);
  // Thiết lập timer để gửi dữ liệu lên Blynk mỗi 1 giây
  timer.setInterval(1000L, sendSensorData);
  
  // Khởi tạo trạng thái ban đầu là đèn đỏ
  changeState(RED_STATE);
}

void loop()
{
  handleButton();
  // Chạy Blynk
  Blynk.run();

  // Chạy timer
  timer.run();

  uint32_t currentMillis = millis();

  // Xử lý nút nhấn
  handleButton();

  // Xử lý cảm biến ánh sáng
  handleLightSensor();

  // Xử lý chế độ đêm (nhấp nháy đèn vàng)
  handleNightMode();

  // Nếu đang ở chế độ đêm hoặc chế độ thủ công, không xử lý đèn giao thông tự động
  if (isNightMode || manualMode)
  {
    return;
  }

  uint32_t stateDuration = currentState == RED_STATE ? RED_DURATION : (currentState == GREEN_STATE ? GREEN_DURATION : YELLOW_DURATION);

  // Xử lý đếm ngược mỗi giây
  if (currentMillis - countdownPreviousMillis >= COUNTDOWN_INTERVAL)
  {
    countdownPreviousMillis = currentMillis;
    if (countdown > 0)
    {
      countdown--;
      displayNumber(countdown);
    }
  }

  // Kiểm tra chuyển trạng thái
  if (currentMillis - stateStartMillis >= stateDuration)
  {
    // Chuyển sang trạng thái tiếp theo
    switch (currentState)
    {
    case RED_STATE:
      changeState(GREEN_STATE);
      break;
    case GREEN_STATE:
      changeState(YELLOW_STATE);
      break;
    case YELLOW_STATE:
      changeState(RED_STATE);
      break;
    }
  }
}