/*
  Blink
 
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 The circuit:
 * LED connected from digital pin 13 to ground.
 
 * Note: On most Arduino boards, there is already an LED on the board
 connected to pin 13, so you don't need any extra components for this example.
 
 
 Created 1 June 2005
 By David Cuartielles
 
 http://arduino.cc/en/Tutorial/Blink
 
 based on an orginal by H. Barragan for the Wiring i/o board

 */

int ledPin =  13;    // LED connected to digital pin 13

// The setup() method runs once, when the sketch starts

void setup()   {                
  // initialize the digital pin as an output:
  pinMode(ledPin, OUTPUT);     
  Serial.begin( 2400 );
  delay( 50 );
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop()                     
{
  char data1 = 5;
  char data2 = 50;
  char cksum = data1 + data2;
  //start byte
  Serial.print( 0xAA, BYTE );
  //first data byte
  Serial.print( data1, BYTE );
  //second data byte
  Serial.print( data2, BYTE );
  //checksum
  Serial.print( cksum, BYTE );
  delay(50);
}
