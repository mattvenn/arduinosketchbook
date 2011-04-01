#include "WProgram.h"
void setup();
void loop();
int ledPin =  8;    // LED connected to digital pin 13
int i = 0;
// The setup() method runs once, when the sketch starts

void setup()   {                
  // initialize the digital pin as an output:
  pinMode(ledPin, OUTPUT);     
  Serial.begin( 19200 );
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop()                     
{
  i ++;

  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(5);
  Serial.print( "i is: " );
  Serial.println( i );
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(5);
}






int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

