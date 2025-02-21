#include <Arduino.h>

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
const uint32_t RED_DURATION = 20000;      // Thời gian đèn đỏ: 20 giây
const uint32_t GREEN_DURATION = 10000;    // Thời gian đèn xanh: 10 giây
const uint32_t YELLOW_DURATION = 3000;    // Thời gian đèn vàng: 3 giây
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
const uint32_t debounceDelay = 50;     // Thời gian debounce: 50ms
bool isNightMode = false;              // Chế độ đêm (khi trời tối)
bool yellowBlinkState = false;         // Trạng thái nhấp nháy đèn vàng

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

// Khai báo prototype cho các hàm
void turnOffDisplay();
void displayNumber(uint8_t number);
void updateDisplayState();
void changeState(TrafficState newState);
void handleButton();
void handleLightSensor();
void handleNightMode();

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
}

void changeState(TrafficState newState)
{
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
      countdown = RED_DURATION / 1000;
      break;
    case GREEN_STATE:
      digitalWrite(GREEN_PIN, HIGH);
      countdown = GREEN_DURATION / 1000;
      break;
    case YELLOW_STATE:
      digitalWrite(YELLOW_PIN, HIGH);
      countdown = YELLOW_DURATION / 1000;
      break;
    }
  }

  currentState = newState;
  stateStartMillis = millis();
  updateDisplayState();
}

void handleButton()
{
  // Đọc trạng thái nút nhấn
  int reading = digitalRead(BUTTON_PIN);

  // Kiểm tra xem nút nhấn có thay đổi trạng thái không
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  // Nếu đã qua thời gian debounce và trạng thái ổn định
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // Nếu trạng thái nút nhấn đã thay đổi
    if (reading != buttonState)
    {
      buttonState = reading;

      // Nếu nút được nhấn (LOW với INPUT_PULLUP)
      if (buttonState == LOW)
      {
        // Đảo trạng thái lựa chọn hiển thị của người dùng
        displayUserChoice = !displayUserChoice;
        blueLedOn = !blueLedOn;

        // Cập nhật LED xanh dương ngay lập tức
        digitalWrite(BLUE_PIN, blueLedOn ? HIGH : LOW);

        // Cập nhật trạng thái hiển thị màn hình LED dựa trên lựa chọn mới
        updateDisplayState();

        // In ra trạng thái debug
        Serial.print("Display user choice: ");
        Serial.println(displayUserChoice ? "ON" : "OFF");
        Serial.print("Actual display: ");
        Serial.println(displayOn ? "ON" : "OFF");
      }
    }
  }

  // Lưu trạng thái nút nhấn hiện tại để so sánh trong lần tiếp theo
  lastButtonState = reading;
}

void handleLightSensor()
{
  // Đọc giá trị từ cảm biến ánh sáng (LOW khi trời tối, HIGH khi trời sáng)
  bool isDark = digitalRead(LIGHT_SENSOR_PIN) == LOW;

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

void setup()
{
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
  Serial.begin(115200);
  Serial.println("Traffic Light System Starting...");

  // Khởi tạo trạng thái ban đầu là đèn đỏ
  changeState(RED_STATE);
}

void loop()
{
  uint32_t currentMillis = millis();

  // Xử lý nút nhấn
  handleButton();

  // Xử lý cảm biến ánh sáng
  handleLightSensor();

  // Xử lý chế độ đêm (nhấp nháy đèn vàng)
  handleNightMode();

  // Nếu đang ở chế độ đêm, không xử lý đèn giao thông bình thường
  if (isNightMode)
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