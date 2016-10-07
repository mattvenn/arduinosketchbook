#define LED 25 //PA2
#define BUTTON 12

#include <Wire.h>


char button_data = 0;
void receiveEvent(int howMany) {
    char c = Wire.read(); // receive byte as a character
    if(c)
        digitalWrite(LED, HIGH);
    else
        digitalWrite(LED, LOW);

}

void requestEvent() 
{
    Wire.write(button_data); 
}

void setup()
{
    pinMode(LED,OUTPUT);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);

    pinMode(BUTTON,INPUT);
    digitalWrite(BUTTON,HIGH);

    Wire.begin(8);                // join i2c bus with address #8
    Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent); // register event
}

void loop()
{
    if(digitalRead(BUTTON) == LOW)
        button_data = 0x01;
    else
        button_data = 0x00;
}
