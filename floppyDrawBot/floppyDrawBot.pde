#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//pin defs
#define stepDir 3 //which direction to step the read/write head
#define stepPulse 6 //when we pulse this the stepper moves one step
#define motorOn 4 //turn the platter motor on
#define motorPulse 5 //pulsing this sets the speed of the platter
#define motorClock 2 //this is an input from the floppy that pulses once per revolution of the platter
#define LEDPIN 13

//prog defs
#define STEPS 75 //maximum steps the stepper can make
#define MINSPEED 1
#define MAXSPEED 30
#define FORWARD false
#define BACKWARD true

//#define PLATTER_INT 

//globals
boolean draw = true;
volatile boolean startPoint = false; //for some experimental interrupt stuff
volatile unsigned int revCount = 0;
void setup()
{
  pinMode( motorPulse, OUTPUT );
  pinMode( stepDir, OUTPUT );
  pinMode( stepPulse, OUTPUT );
  pinMode( motorOn, OUTPUT );
  pinMode( LEDPIN, INPUT );
  pinMode( motorClock, INPUT );
  
  digitalWrite( motorOn, LOW ); 
  Serial.begin(9600);
  Serial.println( "floppy drawbot starting" );

// this isn't working because there isn't a clean enough signal from the floppy
  #ifdef PLATTER_INT
    attachInterrupt( 0, revInt, FALLING );
  #endif
  
  //sets up the timer for sending pulses to the platter motor
  setupTimer();
}

#ifdef PLATTER_INT
//isr for the platter index count
void revInt()
{
  revCount ++;
  if( revCount > 8 )
  {
    revCount = 0;
    startPoint = true;
  }
}
#endif

void loop()
{
  if( Serial.available() ) 
  {
    char command = Serial.read();
    Serial.println( command );
    switch( command )
    {
      case 'g':
        draw = true;
        Serial.println( "go" );
        digitalWrite( motorOn, false ); //switch platter motor on
        break;
      case 's':
        draw = false;
        Serial.println( "stop" );
        digitalWrite( motorOn, true ); //switch platter motor off
        break;
    }
  }
    
  //speed to move the pen arm  
  int speed = map(analogRead(0),0,1024,MINSPEED,MAXSPEED);
  
  if( draw )
  {
    #ifdef PLATTER_INT
    if( startPoint )
    {
      startPoint = false;
    #endif
      doSteps(FORWARD, STEPS, speed); //random(MINSPEED,MAXSPEED));
      doSteps(BACKWARD, STEPS, speed ); //random(MINSPEED,MAXSPEED));
    #ifdef PLATTER_INT
    }
    #endif
  }
}

void doSteps( boolean dir, int steps, int stepDelay )
{
  digitalWrite( stepDir, dir );
  for( int i = 0; i < steps; i ++ )
  {
    digitalWrite( stepPulse, LOW );
    _delay_ms( stepDelay );
    digitalWrite( stepPulse, HIGH );
    _delay_ms( stepDelay );
  }
}
