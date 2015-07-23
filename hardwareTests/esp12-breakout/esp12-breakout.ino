#include <ESP8266WiFi.h>
#include "secrets.h"

#define PINS 9
int pins[PINS]={0,2,4,5,12,13,14,15,16};
void setup() {
  // put your setup code here, to run once:
  for(int p=0; p<PINS; p++)
    pinMode(pins[p],OUTPUT);
    Serial.begin(9600);
    Serial.println("Wifi Temperature Sensor");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


}

void loop() {
  // put your main code here, to run repeatedly:
for(int p=0; p<PINS; p++)
    digitalWrite(pins[p],HIGH);
delay(500);
for(int p=0; p<PINS; p++)
    digitalWrite(pins[p],LOW);
delay(500);

  
}
