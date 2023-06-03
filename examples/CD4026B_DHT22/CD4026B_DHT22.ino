/*
https://www.ti.com/product/CD4026B
*/

#include <DHT.h>

#define CLK_PIN 2
#define DEI_PIN 3
#define RST_PIN 4
#define DHTPIN 5

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
int humidity;

void displayNumber(int num)
{
  digitalWrite(DEI_PIN, LOW);
  digitalWrite(RST_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(RST_PIN, LOW);
  for (int i = 0; i < num; ++i)
  {
    digitalWrite(CLK_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(CLK_PIN, LOW);
    delayMicroseconds(10);
  }
  digitalWrite(DEI_PIN, HIGH);
}

void setup() 
{
  dht.begin();
  Serial.begin(115200);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DEI_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(DEI_PIN, HIGH);
}

void loop() 
{
  int h = dht.readHumidity();
  if (humidity != h)
  {
    humidity = h;
    displayNumber(h);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F("%"));
    delay(2000);
  }
}
