#include <Arduino.h>

// Định nghĩa chân kết nối cho đèn giao thông
const uint8_t RED_PIN = 16, YELLOW_PIN = 17, GREEN_PIN = 5;

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

// Các biến trạng thái
TrafficState currentState = RED_STATE; // Trạng thái hiện tại
uint32_t stateStartMillis = 0;         // Thời điểm bắt đầu trạng thái
uint32_t countdownPreviousMillis = 0;  // Thời điểm đếm ngược gần nhất
uint8_t countdown = 0;                 // Giá trị đếm ngược hiện tại

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

void displayNumber(uint8_t number)
{
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

void changeState(TrafficState newState)
{
  // Tắt tất cả đèn
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);

  // Bật đèn mới và cập nhật trạng thái
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

  currentState = newState;
  stateStartMillis = millis();
  displayNumber(countdown);
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

  // Khởi tạo trạng thái ban đầu là đèn đỏ
  changeState(RED_STATE);

  Serial.begin(115200);
}

void loop()
{
  uint32_t currentMillis = millis();
  uint32_t stateDuration = currentState == RED_STATE ? RED_DURATION : currentState == GREEN_STATE ? GREEN_DURATION
                                                                                                  : YELLOW_DURATION;

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