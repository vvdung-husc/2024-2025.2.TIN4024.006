#include <Arduino.h>
#include <TimeLib.h> 

#define GREEN_LED 16  // Chân kết nối đèn xanh
#define YELLOW_LED 17 // Chân kết nối đèn vàng
#define RED_LED 5     // Chân kết nối đèn đỏ

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.begin(115200); 
  
  // Cấu hình thời gian mặc định nếu chưa có nguồn thời gian thực
  setTime(6, 30, 0, 1, 1, 2025); 
}

void loop() {
  // Lấy thời gian hệ thống (giờ)
  int currentHour = hour(); 
  int currentMinute = minute();
  
  Serial.print("Giờ hiện tại: ");
  Serial.print(currentHour); Serial.print(":"); Serial.println(currentMinute);// Hiển thị giờ hiện tại
  
  if (currentHour >= 6 && currentHour < 22) {
    // Nếu thời gian từ 6h - 22h thì sáng 3 đèn
    // Đèn xanh sáng trong 10 giây
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
    Serial.println("Bật đèn xanh: ");
    for (int i = 10; i > 0; i--) {
      
      Serial.print("Đèn xanh: ");
      Serial.print(i);
      Serial.println(" giây");
      delay(1000);
    }

    // Đèn vàng sáng trong 3 giây
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    Serial.println("Bật đèn vàng: ");
    for (int i = 3; i > 0; i--) {
      Serial.print("Đèn vàng: ");
      Serial.print(i);
      Serial.println(" giây");
      delay(1000);
    }

    // Đèn đỏ sáng trong 7 giây
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    Serial.println("Bật đèn đỏ: ");
    for (int i = 7; i > 0; i--) {
      Serial.print("Đèn đỏ: ");
      Serial.print(i);
      Serial.println(" giây");
      delay(1000);
    }
  } else {
    // Nếu thời gian từ 22h trở đi thì chỉ sáng đèn vàng
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    
    Serial.println("Đèn vàng sáng từ 22h - 6h");
    delay(1000);
    
  }
}
