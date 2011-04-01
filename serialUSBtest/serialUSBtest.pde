int ledPin =  13;    // LED connected to digital pin 13
int knob1,knob2,oldKnob1, oldKnob2 = 0;

int SWITCH1 = 3;
int SWITCH2 = 2;
#include <Button.h>
int thresh = 9;
// The setup() method runs once, when the sketch starts

Button button1 = Button( SWITCH1, PULLUP );
Button button2 = Button( SWITCH2, PULLUP );

void setup()   {                
  // initialize the digital pin as an output:
  pinMode(ledPin, OUTPUT);     
  pinMode(SWITCH1, INPUT );
  digitalWrite( SWITCH1, HIGH );
    pinMode(SWITCH2, INPUT );
  digitalWrite( SWITCH2, HIGH );
  Serial.begin( 115200 );
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop()                     
{
delay( 50 );  
  knob1 = analogRead(0);
  knob2 = analogRead(1);

  if( abs( knob1 - oldKnob1 ) > thresh )
  {
  oldKnob1 = knob1; 
  Serial.print( "k1" );
  Serial.println( knob1, DEC );
  }
  
   if( abs( knob2 - oldKnob2 ) > thresh )
   {
     oldKnob2 = knob2; 
  Serial.print( "k2" );
  Serial.println( knob2, DEC );    

   }

  if( button1.uniquePress() )
  {
    Serial.println( "b1" );
  }
  if( button2.uniquePress() )
  {
    Serial.println( "b2" );
  }
  /*
  byte b1 = (byte)knob1;
  byte b2 = (byte)(knob1 >> 8);
  digitalWrite(ledPin, HIGH);   // set the LED on
  Serial.print( b2 );
  Serial.print( b1 );

  b1 = (byte)knob2;
  b2 = (byte)(knob2 >> 8);

  Serial.print( b2 );
  Serial.print( b1 );
  byte b3 = button1.wasPressed() ? 1 : 0;
  Serial.println( b3 );

  digitalWrite(ledPin, LOW);    // set the LED off
  }
*/
}





