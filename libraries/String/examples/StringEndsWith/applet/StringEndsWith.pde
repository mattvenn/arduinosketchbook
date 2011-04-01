/*
  String setCharAt() function demo
 
 Demonstrates the setCharAt function of the String library. Inserts incoming
 text into an existing string, always between the two end substrings.
 
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
  if(Serial.available() > 0) {
    // read the incoming data as a char:
    char inChar = Serial.read();
    inString.append(inChar);
    
  }

  if (inString.endsWith("\r\n")) {
    Serial.print("Windows ending");
  } 
  else if  (inString.endsWith("\r")) {
    Serial.print("Mac ending"); 
  }
}
