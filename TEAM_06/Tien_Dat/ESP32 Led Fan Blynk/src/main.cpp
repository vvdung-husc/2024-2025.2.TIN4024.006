#define BLYNK_TEMPLATE_ID "TMPL6wEw55cZ2"
#define BLYNK_TEMPLATE_NAME "DieuKhienThietBi"
#define BLYNK_AUTH_TOKEN "s7ghqykBcNP41aRs48o_9enl0zEJOv7b"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DEN 2
#define QUAT 4


#define BTN_DEN 5
#define BTN_QUAT 18

bool denBat = false;
bool quatBat = false;

// 💡 Khai báo nguyên mẫu hàm trước loop()
void docNutBam();

void setup() {
  Serial.begin(115200);

  pinMode(DEN, OUTPUT);
  pinMode(QUAT, OUTPUT);

  pinMode(BTN_DEN, INPUT_PULLUP);
  pinMode(BTN_QUAT, INPUT_PULLUP);

  digitalWrite(DEN, LOW);
  digitalWrite(QUAT, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  docNutBam();
}

void docNutBam() {
  static bool lastBtnDen = HIGH;
  static bool lastBtnQuat = HIGH;

  bool btnDen = digitalRead(BTN_DEN);
  bool btnQuat = digitalRead(BTN_QUAT);

  if (btnDen == LOW && lastBtnDen == HIGH) {
    denBat = !denBat;
    digitalWrite(DEN, denBat);
    Blynk.virtualWrite(V1, denBat);
  }
  if (btnQuat == LOW && lastBtnQuat == HIGH) {
    quatBat = !quatBat;
    digitalWrite(QUAT, quatBat);
    Blynk.virtualWrite(V2, quatBat);
  }

  lastBtnDen = btnDen;
  lastBtnQuat = btnQuat;
}

// Điều khiển từ Blynk
BLYNK_WRITE(V1) {
  denBat = param.asInt();
  digitalWrite(DEN, denBat);
}
BLYNK_WRITE(V2) {
  quatBat = param.asInt();
  digitalWrite(QUAT, quatBat);
}
