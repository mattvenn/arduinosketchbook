  /* 
todo:
 - what kind of precision do we have on the pen? will we get slopes or squares?
 + calibration,
 - check calibration!
 - calculation of step sizes for roller and pen
  -find a way to find where the pen is at startup
    - low torque one motor, high the other. Pull till switch triggers. Magnet on gondola and strings.
 
 */

//where we define all the paramaters of the robot
#include "robotdefs.h"

//pattern type
//#define DRAW_ENERGY_CIRCLES
//#define DRAW_DAY_SPIRAL
#define ARCOLA_WEEK_CIRCLES

#include <Stepper.h>
#include <NewSoftSerial.h>
#include <TimedAction.h>

#define LEFT 0
#define RIGHT 1

const float circumference = 3.1415 * DIAMETER;
const int StepUnit = stepsPerRevolution / circumference;   

// Approximate dimensions (in steps) of the total drawing area
const int w= MOTOR_DIST_CM*StepUnit;
const int h= MOTOR_DIST_CM*StepUnit; 

const int ceiling = h / 4; //5; // 10*StepUnit;
const int margin = w / 5; //4;

// Coordinates of current (starting) point
int x1= w/2;
int y1= h/2;

boolean steppersOn = false;

// Approximate length of strings from marker to staple
int a1= sqrt(pow(x1,2)+pow(y1,2));
int b1= sqrt(pow((w-x1),2)+pow(y1,2));

//globals

unsigned long lastTime;
int penPos, lastPenPos;
boolean stepping = false;
int drawCount = 0;

#define DEBUG

NewSoftSerial xbeeSerial(XBEERX, XBEETX);
#ifdef XBEE

TimedAction ActionCheckXbeeData = TimedAction( 1000, checkXbeeData);
#endif
#ifdef DEBUG
TimedAction ActionCheckSerialData = TimedAction( 200, checkSerialData);
#endif

TimedAction ActionTurnOffSteppers = TimedAction( 500, turnOffSteppers );

Stepper leftStepper(stepsPerRevolution, STEPLPIN1, STEPLPIN2, STEPLPIN3, STEPLPIN4 );
Stepper rightStepper(stepsPerRevolution, STEPRPIN1, STEPRPIN2, STEPRPIN3, STEPRPIN4 );

void setup() {
  pinMode( STATUS_LED, OUTPUT );
  pinMode( STEP_PWM, OUTPUT );
  pinMode(OPTO_ROLLER,INPUT);
  pinMode(OPTO_PEN,INPUT);
  
  pinMode( TENS_L, INPUT );
  digitalWrite( TENS_L, HIGH );
  pinMode( TENS_R, INPUT );
  digitalWrite( TENS_R, HIGH );
  
  //digitalWrite(OPTO_ROLLER,LOW);
 // digitalWrite(OPTO_PEN,LOW);
  // set the speed at 60 rpm:
  leftStepper.setSpeed(stepSpeed );
  rightStepper.setSpeed(stepSpeed );
  
  setPwmFrequency( STEP_PWM, 1 ); //set to 32khz / 1
  
  analogWrite( STEP_PWM, PWM_HIGH );
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println( "energy plotter startup" );  
  #ifdef XBEE
  xbeeSetup();
  #endif

      digitalWrite( STATUS_LED, HIGH );
      delay(1000);
      digitalWrite( STATUS_LED, LOW );
  
//  calibrate();

}
unsigned int counter = 0;

void loop()
{
 ActionCheckSerialData.check();
 //if steppers not in use, then turn power off
 if( stepping == false )
   turnOffSteppers();
}

void burnTest(int number)
{
  Serial.println( "starting burn test" );
  for( int i = 0; i < number ; i ++ )
 { 
  Serial.println( i );
  Serial.println( "move to 34,34" );
 
  moveTo( 34 * StepUnit, 30 * StepUnit);
  turnOffSteppers();
  delay(100);
  Serial.println( "move to 32,32" );
  moveTo( 34* StepUnit, 34 * StepUnit);
  turnOffSteppers();
  delay(100);
 }
}
#ifdef DEBUG
void checkSerialData()
{
  #ifdef XBEE
  if( xbeeSerial.available() )
  {
    digitalWrite( STATUS_LED, HIGH );
    char command = xbeeSerial.read();
   switch( command )
    {
      case 'e':
      {
        Serial.println( "got energy command" );
        delay(200 * delayFactor);
        int energy = xbeeserReadInt();
        int minute = xbeeserReadInt();
        int ckSum = xbeeserReadInt();
        
        if( ckSum != energy + minute )
        {
          Serial.print( "bad checksum" );
          break;
        }

       
        Serial.print( "set energy to: " );
        Serial.print( energy );
        Serial.print( " at " );
        Serial.println( minute );
     //   drawEnergy( energy, minute );
        xbeeSerial.println("OK");
        Serial.println( "OK" );
        xbeeSerial.flush();
        break;
      }
      default:
        Serial.print( "bad command: " );
        Serial.println( command );
        break;
    }
    digitalWrite( STATUS_LED, LOW );
  } 
  #endif
  if( Serial.available() )
  {
        digitalWrite( STATUS_LED, HIGH );
    char command = Serial.read();
    switch( command )
    {
      case 'c':
        calibrate();
        break;
      case 'b':
        burnTest( serReadInt() );
        break;
      case 'e':
      {
        int energy = serReadInt();
        int day = serReadInt();
        int hour = serReadInt();
        drawEnergy( energy, day, hour );
        Serial.print( "set energy to: " );
        Serial.print( energy );
        Serial.print( " at " );
        Serial.print( day );
      Serial.print( ",");
        Serial.println( hour );
        break;
      }
      case 'l':
        step( LEFT, serReadInt() );
        break;
      case 'r':
         step( RIGHT, serReadInt() );
         break;
      case 'v': //draw a straight line
      {
        int x = serReadInt();
        //x *= StepUnit;
        int y = serReadInt();
        //y *= StepUnit;
        drawLine( x1, y1,  x, y );        
        break;
      }
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
      case 'z':
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
        digitalWrite( STATUS_LED, LOW );
  }
}
#endif
