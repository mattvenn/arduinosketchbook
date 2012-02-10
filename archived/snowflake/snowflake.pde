int ledPin =  13;    // LED connected to digital pin 13
int knob1,knob2,oldKnob1, oldKnob2 = 0;

int SWITCH1 = 2;
int SWITCH2 = 3;
#define SWITCH1LED 5
#define SWITCH2LED 4
#define PRINTPAPER 6

#include <Button.h>
int thresh = 9;
// The setup() method runs once, when the sketch starts

Button button1 = Button( SWITCH1, PULLUP );
Button button2 = Button( SWITCH2, PULLUP );
Button printButton = Button( PRINTPAPER, PULLUP  );

void setup()   {                
  // initialize the digital pin as an output:
  pinMode(ledPin, OUTPUT);    

  pinMode(SWITCH1, INPUT );
  pinMode(SWITCH2, INPUT );
  pinMode(PRINTPAPER, INPUT );
  pinMode(SWITCH1LED, OUTPUT );
  pinMode(SWITCH2LED, OUTPUT );



  digitalWrite(SWITCH1LED, LOW );
  digitalWrite(SWITCH2LED, LOW );

  digitalWrite(PRINTPAPER, LOW );
  digitalWrite( SWITCH1, HIGH );
  digitalWrite( SWITCH2, HIGH );
  printButton.isPressed();
  Serial.begin( 115200 );
  Serial.flush();
  while( Serial.available() )
    Serial.read();
  Serial.println( "started" );
}

// the loop() method runs over and over again,
// as long as the Arduino has power

void loop()                     
{
  delay( 50 );  
  knob1 = analogRead(5);
  knob2 = analogRead(3);

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
/*  
  if(digitalRead( PRINTPAPER ))
  {
    Serial.println( "p" );
  }
  */
  //this seems required to get stateChanged() to work
  printButton.isPressed();
  /*
  if( printButton.uniquePress() )
  {
    Serial.println( "unique press" );
  }*/
  if( printButton.stateChanged() )
  {
    if( printButton.isPressed() )
      Serial.println( "paper 1" );
    else
      Serial.println( "paper 0" );
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
  serial reading stuff
  */
  if( Serial.available() >= 3 )
  {
    char command[3];
    for( int i = 0 ; i < 3 ; i ++ )
    {
      command[i] = Serial.read();
    }

    if( command[0] != 'b' )
    {
      Serial.println( "unknown command" );
      Serial.flush();
    }
    else
    {
      Serial.print( "got: " );
      Serial.println( command );
      if( command[1] == '1' )
      {
        if( command[2] == '0' )

          digitalWrite( SWITCH1LED, LOW );

        else if( command[2] == '1' )
          digitalWrite( SWITCH1LED, HIGH );
      }
      else if( command[1] == '2' )
      {
        if( command[2] == '0' )
          digitalWrite( SWITCH2LED, LOW );
        else if( command[2] == '1' )
          digitalWrite( SWITCH2LED, HIGH );
      }  
    }
  }
}






