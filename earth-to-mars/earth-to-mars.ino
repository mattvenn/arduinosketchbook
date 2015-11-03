#define H_SENSE A6
#define H_HEAT 11
#define DHT_PIN 12
#define DHTTYPE DHT11
#include "DHT.h"
DHT dht(DHT_PIN, DHTTYPE);

void setup() 
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    analogWrite(H_HEAT,0);
    dht.begin();
}

void loop() 
{
    float temp = dht.readTemperature();
    Serial.println(temp);
    read_h2();
    delay(500);
}

void read_h2()
{
    //ramp up heater
    for(int i=0; i< 255; i++)
    {
        delay(1);
        analogWrite(H_HEAT,i);
    }
    delay(500);
    Serial.println(analogRead(A6));
    //turn off heater
    analogWrite(H_HEAT,0);
}
