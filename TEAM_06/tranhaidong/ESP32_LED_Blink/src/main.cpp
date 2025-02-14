#include <Arduino.h>

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 

  Serial.println("IoT wel co me");


}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t i = 0; ++i;
  delay(1000);
  ulong t = millis();
  Serial.print("loop i = ");Serial.print(i);
  Serial.print("  Timer = ");Serial.println(t);
}

