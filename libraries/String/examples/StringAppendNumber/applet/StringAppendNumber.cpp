/*
  String append function demo
 
 Demonstrates the Append function of the String library using ints as input.
 Appends the ASCII representation of a number to a string.
 
 The circuit:
 * an analog input is attached to analog input 0.  
 
 by Tom Igoe
 created 8 Feb 2009
 */


#include <WString.h>                // include the String library

#include "WProgram.h"
void setup();
void loop ();
String dataString;       // allocate a new String

void setup() {
  // open the serial port:
  Serial.begin(9600);
  // Say hello:
  Serial.print("String Library version: ");
  Serial.println(dataString.version());
}

void loop () {
  dataString = "Anlog Reading: ";  
  // Read the analog input:
  int analogValue = analogRead(0);
    // get the millis():
    long timeStamp = millis();
    // add the reading to the string:
  dataString.append(analogValue);
  
  // add some more text:
  dataString.append("\t time since program started: ");
  // add the timestamp:
  dataString.append(timeStamp);
  // print the string:
  Serial.println(dataString);

}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

