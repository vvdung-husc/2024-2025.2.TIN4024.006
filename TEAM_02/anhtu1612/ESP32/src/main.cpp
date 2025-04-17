#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 23
#define OLED_SCL 22
#define OLED_ADDR 0x3C

// PIR
#define PIR_IN_PIN 14
#define PIR_OUT_PIN 27

// Trạng thái cảm biến
enum SensorState {
  NONE,
  PIR1_FIRST,
  PIR2_FIRST
};

// OLED và Wire
TwoWire I2CBus = TwoWire(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2CBus, -1);

// Biến đếm
int countIn = 0;
int countOut = 0;

// Logic phát hiện chuyển động
SensorState lastTriggered = NONE;
unsigned long triggerTime = 0;
const unsigned long MAX_DURATION = 2000;  // 2 giây timeout

// Phát hiện cạnh lên
bool lastPirIn = LOW;
bool lastPirOut = LOW;

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Vao: ");
  display.println(countIn);
  display.setCursor(0, 32);
  display.print("Ra:  ");
  display.println(countOut);
  display.display();
}

void setup() {
  Serial.begin(115200);

  // Khởi tạo OLED
  I2CBus.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Khong tim thay OLED!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Khoi dong he thong...");
  display.display();
  delay(1000);

  pinMode(PIR_IN_PIN, INPUT);
  pinMode(PIR_OUT_PIN, INPUT);

  updateDisplay();
}

void loop() {
  bool pirInNow = digitalRead(PIR_IN_PIN);
  bool pirOutNow = digitalRead(PIR_OUT_PIN);
  unsigned long currentTime = millis();

  // Phát hiện cạnh lên
  bool pirInRising = (pirInNow == HIGH && lastPirIn == LOW);
  bool pirOutRising = (pirOutNow == HIGH && lastPirOut == LOW);

  switch (lastTriggered) {
    case NONE:
      if (pirInRising) {
        lastTriggered = PIR1_FIRST;
        triggerTime = currentTime;
        Serial.println("Phat hien nguoi o PIR1");
      } else if (pirOutRising) {
        lastTriggered = PIR2_FIRST;
        triggerTime = currentTime;
        Serial.println("Phat hien nguoi o PIR2");
      }
      break;

    case PIR1_FIRST:
      if (pirOutRising) {
        if (currentTime - triggerTime <= MAX_DURATION) {
          countIn++;
          Serial.println("Xac nhan nguoi vao!");
          updateDisplay();
        } else {
          Serial.println("Het thoi gian cho PIR2");
        }
        lastTriggered = NONE;
      } else if (currentTime - triggerTime > MAX_DURATION) {
        Serial.println("Khong xac nhan nguoi vao (timeout)");
        lastTriggered = NONE;
      }
      break;

    case PIR2_FIRST:
      if (pirInRising) {
        if (currentTime - triggerTime <= MAX_DURATION) {
          countOut++;
          Serial.println("Xac nhan nguoi ra!");
          updateDisplay();
        } else {
          Serial.println("Het thoi gian cho PIR1");
        }
        lastTriggered = NONE;
      } else if (currentTime - triggerTime > MAX_DURATION) {
        Serial.println("Khong xac nhan nguoi ra (timeout)");
        lastTriggered = NONE;
      }
      break;
  }

  // Reset phát hiện cạnh nếu cảm biến đã về LOW
  if (pirInNow == LOW) {
    lastPirIn = LOW;
  } else if (pirInNow == HIGH && lastPirIn == LOW) {
    lastPirIn = HIGH;
  }

  if (pirOutNow == LOW) {
    lastPirOut = LOW;
  } else if (pirOutNow == HIGH && lastPirOut == LOW) {
    lastPirOut = HIGH;
  }

  delay(50);  // Giảm nhiễu
}
