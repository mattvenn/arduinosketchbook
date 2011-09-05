
/* 
todo:
 - what kind of precision do we have on the pen? will we get slopes or squares?
 - calibration
 - calculation of step sizes for roller and pen
 */

#include <Stepper.h>
#include <NewSoftSerial.h>
#include <TimedAction.h>

//system defs
#define stepsPerRevolution 200  // change this to fit the number of steps per revolution
#define LOOP_PERIOD 400.0 //seconds
#define MAX_PEN_STEPS 2000
#define MAX_ENERGY 4000 //W
#define stepSpeed 40

// Approximate number of steps per cm, calculated from radius of spool
// and the number of steps per radius
float diameter = 1.24; //1.29
float circumference = 3.1415 * diameter;

int StepUnit = stepsPerRevolution / circumference;   

// Approximate dimensions (in steps) of the total drawing area
int w= 68*StepUnit;
int ceiling = 10*StepUnit;
int h= 34 * StepUnit + ceiling;


// Coordinates of current (starting) point
int x1= w/2;
int y1= h;

// Approximate length of strings from marker to staple
int a1= sqrt(pow(x1,2)+pow(y1,2));
int b1= sqrt(pow((w-x1),2)+pow(y1,2));

//globals
float millisPerStep;
unsigned long lastTime;
int energy;
int penPos, lastPenPos;
boolean draw= false;
int drawCount = 0;

#define STATUS_LED 4                                    
#define OPTO_ROLLER A4
#define OPTO_PEN A5
#define PWM_RED 5 
#define PWM_GREEN 6
#define XBEETX 3
#define XBEERX 4




#define DEBUG

#ifdef XBEE
NewSoftSerial xbeeSerial(XBEETX, XBEERX);
TimedAction ActionCheckXbeeData = TimedAction( 1000, checkXbeeData);
#endif
#ifdef DEBUG
TimedAction ActionCheckSerialData = TimedAction( 1000, checkSerialData);
#endif

//TimedAction ActionStatusBlink = TimedAction( 500, statusLED );
//TimedAction ActionDraw = TimedAction( 1000, draw );
//TimedAction ActionLEDColour = TimedAction( 1000, LEDColour );
// initialize the stepper library on pins 8 through 11:
// 3 brown
// 4 red
// 5 white
// 6 green
Stepper leftStepper(stepsPerRevolution, A0,A1,A2,A3);            
Stepper rightStepper(stepsPerRevolution, 9,10,11,12);            

void setup() {
  pinMode( STATUS_LED, OUTPUT );
  pinMode( PWM_RED, OUTPUT );
  pinMode( PWM_GREEN, OUTPUT );
  pinMode(OPTO_ROLLER,INPUT);
  pinMode(OPTO_PEN,INPUT);
  //digitalWrite(OPTO_ROLLER,LOW);
 // digitalWrite(OPTO_PEN,LOW);
  // set the speed at 60 rpm:
  leftStepper.setSpeed(stepSpeed);
  rightStepper.setSpeed(stepSpeed);
  
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println( "energy plotter startup" );  
  #ifdef XBEE
  xbeeSetup();
  #endif
  Serial.println( x1 );
  Serial.println( y1 );
 // calibrate();
}


void loop()
{
  #ifdef DEBUG
  ActionCheckSerialData.check();
  #endif

 

/*
  ActionCheckXbeeData.check();
  ActionStatusBlink.check();
  ActionLEDColour.check();
  
  //ActionDraw.check();
  if( millis() - lastTime > millisPerStep )
  {
    lastTime = millis();
    draw();
  }
*/
}

/*
 // if( digitalRead(OPTO_ROLLER) == HIGH )
  {
    
    Serial.println( "high" );
    digitalWrite( STATUS_LED, LOW );
    xaxis.step(1);
    yaxis.step(1);
    digitalWrite( STATUS_LED, HIGH );
  }
}
*/

#ifdef DEBUG
void checkSerialData()
{
  if( Serial.available() )
  {
    char command = Serial.read();
    switch( command )
    {
      case 'g':
        drawEnergy();
        break;
      case 'e':
        energy = serReadInt();
        Serial.print( "set energy to: " );
        Serial.println( energy );
        break;
      case 'l':
        leftStepper.step( serReadInt() );
        break;
      case 'r':
         rightStepper.step( serReadInt() );
         break;
      case 'p':
        Serial.print( "steps per cm: " );
        Serial.println( StepUnit );
        Serial.print( "x1: " );
        Serial.println( x1 / StepUnit);
        Serial.print( "y1: ");
        Serial.println( y1 / StepUnit);
        Serial.print( "a1: " );
        Serial.println( a1 / StepUnit );
        Serial.print( "b1: " );
        Serial.println( b1 / StepUnit );
        break;
      case 'd':
        draw = ! draw;
        break;
      case 'm':
      {
        int x = serReadInt();
        x *= StepUnit;
        int y = serReadInt();
        y *= StepUnit;
        Serial.print( x );
        Serial.print( "," );
        Serial.println( y );
        moveTo( x, y );
        break;
      }
      case 'c':
      {
        int x = serReadInt();
        int y = serReadInt();
        int r = serReadInt();
        int n = serReadInt();
        x *= StepUnit;
        y *= StepUnit;
        r *= StepUnit;
        Serial.print( x );
        Serial.print( "," );
        Serial.print( y );
        Serial.print( "," );
        Serial.println( r );
        drawCircles(n,x,y,r);
      break;
      }
      case 's':
       {
         int s = serReadInt();
         leftStepper.setSpeed( s );
         rightStepper.setSpeed( s );
         Serial.print( "step speed: " );
         Serial.println( s );
         break;
       }
        
    }
  }
}
#endif
