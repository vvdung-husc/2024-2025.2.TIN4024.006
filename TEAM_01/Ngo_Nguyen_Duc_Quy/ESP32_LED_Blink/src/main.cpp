//   #include <Arduino.h>

//   int ledPinred = 5;
//   int ledPinvang = 17;
//   int ledPingxanh = 4;
//   bool IsLed_On = false;
//   ulong led_start = 0;
//   // put function declarations here:

//   void setup() { // đoạn code khởi tạo
//     // put your setup code here, to run once:
//     Serial.begin(115200);

//     Serial.println("IOT welcome");
//     pinMode(ledPinred, OUTPUT);
//     pinMode(ledPinvang, OUTPUT);
//     pinMode(ledPingxanh, OUTPUT);
//   }

//   void Use_Blocking(){
//     digitalWrite(ledPinred, HIGH); // Bật den do
//     Serial.println("RED -> ON");
//     delay(5000); 
//     digitalWrite(ledPinred, LOW);
//     Serial.println("RED -> OFF");
//     digitalWrite(ledPinvang, HIGH);// bat den vang
//     Serial.println("VANG -> ON");
//     delay(5000);
//     digitalWrite(ledPinvang, LOW);
//     Serial.println("VANG -> OFF");
//     digitalWrite(ledPingxanh, HIGH);// bat den xanh
//     Serial.println("XANH -> ON");
//     delay(5000);
//     digitalWrite(ledPingxanh, LOW);
//     Serial.println("XANH -> OFF");
//   }

//   bool iSReady(ulong& ulTimer, uint32_t milisecond){
//     ulong t = millis();
//     if(t - ulTimer < milisecond) return false;
//     ulTimer = t;
//     return true;
//   }

//   void Use_Non_Blocking(){
//     if(!iSReady(led_start,1000)) return;
//     if(!IsLed_On){
//       digitalWrite(ledPinred, HIGH); // Bật
//       Serial.println("NonBlocking LED -> ON");
//     }else{
//       digitalWrite(ledPinred, LOW); // Bật
//       Serial.println("NonBlocking LED -> OFF");
//     }
//     IsLed_On = !IsLed_On;
//   }

//   void loop() { 
//   //  Use_Non_Blocking();
// Use_Blocking();
//     // ulong t = millis();
//     // Serial.print("Timer : ");
//     // Serial.println(t);
//   }
#include <Arduino.h>    

int ledPinred = 5;   
int ledPinvang = 17;   
int ledPingxanh = 4;   

ulong led_start = 0;  
int ledState = 0; // Trạng thái của đèn

void setup() { 
    Serial.begin(115200);  
    Serial.println("IOT welcome"); 
    pinMode(ledPinred, OUTPUT); 
    pinMode(ledPinvang, OUTPUT); 
    pinMode(ledPingxanh, OUTPUT); 

    // Bật đèn đỏ ngay khi khởi động
    digitalWrite(ledPinred, HIGH);
    Serial.println("RED -> ON");
    led_start = millis(); // Cập nhật thời gian bắt đầu
}    

bool isReady(ulong& ulTimer, uint32_t milisecond) { 
    ulong t = millis(); 
    if (t - ulTimer < milisecond) return false; 
    ulTimer = t; 
    return true; 
}    

void Use_Non_Blocking() { 
    if (!isReady(led_start, 5000)) return; // Mỗi đèn sáng trong 5 giây

    // Tắt tất cả đèn trước khi bật đèn tiếp theo
    digitalWrite(ledPinred, LOW);
    digitalWrite(ledPinvang, LOW);
    digitalWrite(ledPingxanh, LOW);

    // Chuyển đèn theo thứ tự: Đỏ -> Vàng -> Xanh
    if (ledState == 0) {
        digitalWrite(ledPinred, HIGH);
        Serial.println("RED -> ON");
    } else if (ledState == 1) {
        digitalWrite(ledPinvang, HIGH);
        Serial.println("VANG -> ON");
    } else if (ledState == 2) {
        digitalWrite(ledPingxanh, HIGH);
        Serial.println("XANH -> ON");
    }

    ledState = (ledState + 1) % 3; // Chuyển trạng thái sang đèn tiếp theo
}    

void loop() {    
    Use_Non_Blocking();  
}

