/*
  String capacity() function demo
 
 Demonstrates the capacity function of the String library. How long
 a given String is differs from how long you let it be when
 you declare it.
 
 To test it, send the microcontroller some characters. 
 
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
    // print the current length:
    Serial.print("String length: ");
    Serial.println(inString.length());
    // print the capacity of the string:
    Serial.print("String capacity: ");
    Serial.println(inString.capacity());
    // print some space:
    Serial.println();
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
