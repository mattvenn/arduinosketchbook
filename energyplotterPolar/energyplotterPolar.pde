
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
float diameter = 1.2;
float circumference = 3.1415 * diameter;

int StepUnit = stepsPerRevolution / circumference;   

// Approximate dimensions (in steps) of the total drawing area
int w= 40*StepUnit;
int h= 40*StepUnit;

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

NewSoftSerial xbeeSerial(XBEETX, XBEERX);

#ifdef DEBUG
TimedAction ActionCheckSerialData = TimedAction( 1000, checkSerialData);
#endif
TimedAction ActionCheckXbeeData = TimedAction( 1000, checkXbeeData);
TimedAction ActionStatusBlink = TimedAction( 500, statusLED );
//TimedAction ActionDraw = TimedAction( 1000, draw );
TimedAction ActionLEDColour = TimedAction( 1000, LEDColour );
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
  xbeeSetup();
  Serial.println( x1 );
  Serial.println( y1 );
 // calibrate();
}


void loop()
{
  #ifdef DEBUG
  ActionCheckSerialData.check();
  #endif

  if( draw )
  {
    drawNext();
  }

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

void drawNext()
{

  if( drawCount < 100 )
  {
    leftStepper.step( -1 );
  }
  else if( drawCount < 200 )
  {
    rightStepper.step( 1 );
  }
  else if( drawCount < 300 )
  {
    leftStepper.step( 1 );
  }
  else if( drawCount < 400 )
  {
    rightStepper.step(-1);
  }
  else
  {
    drawCount = 0;
    Serial.println( "reset" );
  }
  drawCount ++;
}
void LEDColour()
{
  analogWrite( PWM_RED,   map( energy, 0, MAX_ENERGY, 0, 255 ) );
  analogWrite( PWM_GREEN, map( energy, MAX_ENERGY, 0, 0, 255 ) ); 
}
/*
void draw()
{
  //advance roller and pen (at same time?)
  rightStepper.step(1);
  
  penPos = map( energy, 0, MAX_ENERGY, 0, MAX_PEN_STEPS );
  if( penPos > MAX_PEN_STEPS )
    penPos = MAX_PEN_STEPS;
  
  leftStepper.step(penPos - lastPenPos);
  lastPenPos = penPos;  
}
*/
void statusLED()
{
  static boolean ledState = false;
  digitalWrite( STATUS_LED, ledState );
  if( ledState == false )
    ledState = true;
  else
    ledState = false;
}

void calibrate()
{
  Serial.println("calibrating");
 //calibrate roller
  Serial.println( "finding hole in paper..." );
  while( ! limit( OPTO_ROLLER ) )
    rightStepper.step(1);
  Serial.println( "found hole, winding again...");
  int count = 0;
  //ignore opto until we've moved the paper on a bit
  while( limit( OPTO_ROLLER ) )
  {
    rightStepper.step(1);
    count ++;
  }
  while( ! limit( OPTO_ROLLER ) )
  {
    count ++;
    rightStepper.step(1);
  }
  Serial.print( "loop is " ); Serial.print( count ); Serial.println( " steps long" );
  millisPerStep = LOOP_PERIOD / count;
  //LOOP_PERIOD is in secs, so:
  millisPerStep *= 1000;
  Serial.print( "millis per step: " );
  Serial.println( millisPerStep );

 //calibrate energy axis 
 Serial.println( "moving pen to limit..." );
 while( ! limit( OPTO_PEN ) )
   leftStepper.step(-1);
 Serial.println( "done" );

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
        Serial.println( x1 );
        Serial.print( "y1: ");
        Serial.println( y1 );
        break;
      case 'd':
        draw = ! draw;
        break;
      case 'm':
      {
        int x = serReadInt();
        int y = serReadInt();
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
        Serial.print( x );
        Serial.print( "," );
        Serial.print( y );
        Serial.print( "," );
        Serial.println( r );
        drawCircle(x,y,r);
      break;
      }
        
    }
  }
}
#endif
