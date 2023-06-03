
/*
https://www.ti.com/product/CD4026B
*/

constexpr uint8_t CLK_PIN = 2;
constexpr uint8_t DEI_PIN = 3;

void setup() 
{
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DEI_PIN, OUTPUT);
  digitalWrite(DEI_PIN, HIGH);
}

void loop() 
{
  digitalWrite(CLK_PIN, HIGH);
  delay(250);
  digitalWrite(CLK_PIN, LOW);
  delay(250);
}
