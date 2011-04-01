/*
  String endsWith() function demo
 
 Demonstrates the endsWith function of the String library.
 
 To test it, open a serial terminal program
 and answer the question.
 
 by Tom Igoe
 created 8 Feb 2009
 */


#include <WString.h>                // include the String library

#define maxLength 30

String inString = String(maxLength);       // allocate a new String
boolean answerComplete = false;

void setup() {
  // open the serial port:
  Serial.begin(9600);
  // Say hello:
  Serial.print("String Library version: ");
  Serial.println(inString.version());
  // print the question:
  askQuestion();
}

void loop () {
  // get anything the user typed:
  if(Serial.available() > 0) {
    getIncomingChars();
  }
  // if they've finished answering, check the answer:
  if (answerComplete == true) {
    if (inString.equals("monkey")) {
      Serial.println();
      Serial.println("That's right, and don't you say otherwise.");
      Serial.println();
      answerComplete = true;
    } 
    // if they got it wrong, tell them:
    else {
      Serial.println("Wrong. Loser."); 
    }
    // clear the string and ask again:
    inString = "";
    askQuestion(); 
  }

}

void askQuestion() {
  Serial.print("What's the coolest animal? Finish your answer with a !"); 
  answerComplete = false;
}

void getIncomingChars() {
  // read the incoming data as a char:
  char inChar = Serial.read();
  // if it's a !, print the string:
  if (inChar == '!') {
    Serial.println(inString); 
    answerComplete = true;
  } 
  else {
    // if you're not at the end of the string, append
    // the incoming character:
    if (inString.length() < maxLength) {
      inString.append(inChar);
    }
    else {
      // empty the string by setting it equal to the incoming char:
      inString = inChar;
    }
  }
}
