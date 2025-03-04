#include <Arduino.h>
#include <TM1637Display.h>
#include <DHT.h>


#define BLYNK_TEMPLATE_ID "TMPL6W4o1yTLS"
#define BLYNK_TEMPLATE_NAME "ESP32 LED TM1637"
#define BLYNK_AUTH_TOKEN "BTszjclnCFJDK-XORvVccd2XOcQ6hIKG"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



char ssid[] = "Wokwi-GUEST"; 
char pass[] = "";           

#define btnBLED 23 
#define pinBLED 21 

#define CLK 18 
#define DIO 19 

#define DHTPIN 16         
#define DHTTYPE DHT22     
DHT dht(DHTPIN, DHTTYPE); 


float temperature = 0;
float humidity = 0;

ulong currentMiliseconds = 0; 
bool blueButtonON = true;     


TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void updateBlueButton();
void uptimeBlynk();
void readDHT();

void setup()
{
  Serial.begin(115200);
  pinMode(pinBLED, OUTPUT);
  pinMode(btnBLED, INPUT_PULLUP);

  display.setBrightness(0x0f);


  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 

  Serial.println();
  Serial.println("WiFi connected");

  digitalWrite(pinBLED, blueButtonON ? HIGH : LOW);
  Blynk.virtualWrite(V1, blueButtonON); 

  Serial.println("== START ==>");
}

void loop()
{
  Blynk.run(); 

  currentMiliseconds = millis();
  uptimeBlynk();
  updateBlueButton();
  readDHT();
}


bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}
void updateBlueButton()
{
  static ulong lastTime = 0;
  static int lastValue = HIGH;
  if (!IsReady(lastTime, 50))
    return; 
  int v = digitalRead(btnBLED);
  if (v == lastValue)
    return;
  lastValue = v;
  if (v == LOW)
    return;

  if (!blueButtonON)
  {
    Serial.println("Blue Light ON");
    digitalWrite(pinBLED, HIGH);
    blueButtonON = true;
    Blynk.virtualWrite(V1, blueButtonON); 
  }
  else
  {
    Serial.println("Blue Light OFF");
    digitalWrite(pinBLED, LOW);
    blueButtonON = false;
    Blynk.virtualWrite(V1, blueButtonON); 
    display.clear();
  }
}

void uptimeBlynk()
{
  static ulong lastTime = 0;
  if (!IsReady(lastTime, 1000))
    return; 
  ulong value = lastTime / 1000;
  Blynk.virtualWrite(V0, value); 
  if (blueButtonON)
  {
    display.showNumberDec(value);
  }
}


BLYNK_WRITE(V1)
{ 
  blueButtonON = param.asInt(); 
  if (blueButtonON)
  {
    Serial.println("Blynk -> Blue Light ON");
    digitalWrite(pinBLED, HIGH);
  }
  else
  {
    Serial.println("Blynk -> Blue Light OFF");
    digitalWrite(pinBLED, LOW);
    display.clear();
  }
}
void readDHT()
{
  static ulong lastReadTime = 0;
  if (!IsReady(lastReadTime, 2000))
    return; 

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Đọc DHT22 thất bại!");
    return;
  }


  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);

  Serial.print("Nhiệt độ: ");
  Serial.print(temperature);
  Serial.print("°C\tĐộ ẩm: ");
  Serial.print(humidity);
  Serial.println("%");
}