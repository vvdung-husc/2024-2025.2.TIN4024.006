#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Định nghĩa chân cho 3 đèn LED
const int LED1 = 15;
const int LED2 = 2;
const int LED3 = 4;

// Định nghĩa chân cho màn hình LCD I2C
const int SDA_PIN = D7; // Chân SDA của LCD
const int SCL_PIN = D6; // Chân SCL của LCD

// Địa chỉ I2C của LCD (thông thường là 0x27 hoặc 0x3F)
// Bạn có thể sử dụng I2C scanner để tìm địa chỉ nếu không chắc chắn
const int LCD_ADDRESS = 0x3F;

// Khởi tạo đối tượng LCD với địa chỉ I2C, số cột và số hàng
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

void setup()
{
  Serial.begin(115200);
  Serial.println("Bắt đầu setup...");

  // Khởi tạo chân LED là OUTPUT
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // Kiểm tra kết nối I2C
  Serial.println("Bắt đầu Wire...");
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Khởi tạo màn hình LCD...");
  lcd.init();
  lcd.backlight();

  Serial.println("Hiển thị chữ Hello...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello");

  Serial.println("Setup hoàn tất!");
}

void loop()
{
  // Nhấp nháy LED1
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);

  // Nhấp nháy LED2
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, LOW);
  delay(500);

  // Nhấp nháy LED3
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
  delay(500);
}