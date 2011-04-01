/*
  String endsWith() function demo
 
 Demonstrates the endsWith function of the String library.
 
 To test it, send the microcontroller some characters ending with "end". 
 
 by Tom Igoe
 created 8 Feb 2009
 */


#include <WString.h>                // include the String library


#define maxLength 30

#include "WProgram.h"
void setup();
void loop ();
void getIncomingChars();
String inString = String(maxLength);       // allocate a new String

void setup() {
  // open the serial port:
  Serial.begin(9600);
  // Say hello:
  Serial.print("String Library version: ");
  Serial.println(inString.version());
}

void loop () {
  if(Serial.available() > 0) {
   getIncomingChars();
   Serial.println(inString);
  }
  
  if (inString.length() > 2) {
   if (inString.endsWith("end")) {
    Serial.println("That's all folks!"); 
       Serial.println(inString);
    inString = "";
   }
  }
}



void getIncomingChars() {
  // read the incoming data as a char:
  char inChar = Serial.read();
  // if you're not at the end of the string, append
  // the incoming character:
  if (inString.length() < maxLength) {
    inString.append(inChar);
  } 
  else {
    // empty the string by setting it equal to the inoming char:
    inString = inChar;
  }
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

