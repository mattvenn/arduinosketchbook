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
    // read the incoming data as a char:
    char inChar = Serial.read();
    // if you're not at the end of the string, append
    // the incoming character:
    if (inString.length() < maxLength) {
      inString.append(inChar);
      // print the string
      Serial.println(inString);
    } 
    else {
      Serial.println("You've entered the maximum number of characters."); 
    }
  }
}
