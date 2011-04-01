#include <NewSoftSerial.h>
#include <WString.h>

int ledPinR = 9;    // LED connected to digital pin 9
int ledPinG = 10;    // LED connected to digital pin 9
int ledPinB = 11;    // LED connected to digital pin 9
int statusLED = 13;

#define rxPin 2
#define txPin 3

// set up a new serial port
NewSoftSerial mySerial(rxPin, txPin );

unsigned int R,G,B;

void setup()
{ 
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode( statusLED, OUTPUT );
  flashStatus();
  
  mySerial.begin(9600);
  Serial.begin(9600);
  Serial.println( "bluetooth RGB LED controller" );

  R = 255;
  G = 255;
  B = 255;
  updateLED();
} 

void loop()
{ 
  //we expect RGB chars plus a new line
  if( mySerial.available() > 3 )
  {
    R = 255 - mySerial.read();
    G = 255 - mySerial.read();
    B = 255 - mySerial.read();

    //throw away any other chars
    while( mySerial.available() )
      mySerial.read();

    Serial.print( "got RGB: " );

    Serial.print( R, DEC );
    Serial.print("," );
    Serial.print( G, DEC );
    Serial.print("," );
    Serial.println( B, DEC );

    flashStatus();
    updateLED();
  }
} 

void flashStatus()
{
  digitalWrite( statusLED, HIGH );
  delay( 10 );
  digitalWrite( statusLED, LOW );
  delay( 10 );
}

void updateLED()
{
  // sets the value (range from 0 to 255):
  analogWrite(ledPinR, R);         
  analogWrite(ledPinG, G);         
  analogWrite(ledPinB, B);      
}

