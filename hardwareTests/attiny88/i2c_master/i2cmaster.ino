#define LED 13
#include <Wire.h>

void setup() {
  pinMode(LED,OUTPUT);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {

  Wire.requestFrom(8, 1); 
  if (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    if(c)
    {
        digitalWrite(LED,HIGH);
          Wire.beginTransmission(8); // transmit to device #8
          Wire.write(0x01);       
          Wire.endTransmission();    // stop transmitting
      }
    else
    {
        digitalWrite(LED,LOW);
          Wire.beginTransmission(8); // transmit to device #8
          Wire.write(0x00);       
          Wire.endTransmission();    // stop transmitting
      }
  }
}
