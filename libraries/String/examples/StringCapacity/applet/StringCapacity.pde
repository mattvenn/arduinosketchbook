/*
  String charAt() function demo
 
 Demonstrates the contains function of the String library.  indicates
 whether a given string contains the letter "Z" and where it is.
 
 To test it, send the microcontroller some characters.  Include "Z".
 
 by Tom Igoe
 created 8 Feb 2009
 */


#include <WString.h>                // include the String library

#define maxLength 30

String inString = String(maxLength);       // allocate a new String

void setup() {
  // open the serial port:
  Serial.begin(9600);
  // Say hello:
  Serial.print("String Library version: ");
  Serial.println(inString.version());
}

void loop () {
  // See if there's incoming serial data:
  if(Serial.available() > 0) {
    getIncomingChars();
    // print the string
    Serial.println(inString);
  }
  // look for the Z:
  for (int thisChar = 0; thisChar < inString.length(); thisChar++) {
  if (inString.charAt(thisChar) == 'Z') {
    Serial.print("Z is at position ");
    Serial.println(thisChar, DEC);
    // print the string
    Serial.println(inString);
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
