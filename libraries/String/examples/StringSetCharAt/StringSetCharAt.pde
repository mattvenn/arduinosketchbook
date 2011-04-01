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

String nameString = String(maxLength);       // allocate a new String
int characterPosition = 4;                 // where the input characters will be added

void setup() {
  // open the serial port:
  Serial.begin(9600);
  // Say hello:
  Serial.print("String Library version: ");
  Serial.println(nameString.version());
  // give the string an initial value:
  nameString = "***                        ***";
}

void loop () {
  if(Serial.available() > 0) {
    // read the incoming data as a char:
    char inChar = Serial.read();
    // set the incoming character at a position in the string:
    nameString.setCharAt(characterPosition, inChar);

    // increment the character position, keeping it between the
    // *** characters at the start and end of the string:
    if (characterPosition < 25 ) {
      characterPosition++; 
    } 
    else {
      characterPosition = 4; 
      // reset the string:
      nameString = "***                        ***";
    }
    
    // print the string:
    Serial.println(nameString);
  }
}
