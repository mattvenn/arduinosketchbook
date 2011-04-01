/*
  String append function demo
 
 Demonstrates the Append function of the String library using chars as input.
 If you want to add ASCII characters to the end of a String, you have to use
 chars as your input type.  Using ints or longs appends the ASCII value of 
 the input instead.  For an example of that, see the example StringAppendNumber
 
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
