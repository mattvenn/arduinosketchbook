/* 
todo:
 - what kind of precision do we have on the pen? will we get slopes or squares?
 - calibration
 - calculation of step sizes for roller and pen
 
 -solve reseting problem
 -find a way to find where the pen is at startup
 
 RESET problem.
 reducing pwm_high seems to help. 
 changing to a different plug socket made things different.
 */

#include <Stepper.h>
#include <NewSoftSerial.h>
#include <TimedAction.h>

//system defs
#define stepsPerRevolution 200  // change this to fit the number of steps per revolution
#define LOOP_PERIOD 400.0 //seconds
#define MAX_PEN_STEPS 2000
#define MAX_ENERGY 4000 //W
#define stepSpeed 20
#define leftStepDir 1 //these should be set so that the commands l50 and r50 lower the gondola
#define rightStepDir 1
#define LEFT 0
#define RIGHT 1

// Approximate number of steps per cm, calculated from radius of spool
// and the number of steps per radius
float diameter = 1.24; //1.29
float circumference = 3.1415 * diameter;
boolean steppersOn = false;
int StepUnit = stepsPerRevolution / circumference;   

// Approximate dimensions (in steps) of the total drawing area
int w= 68*StepUnit;

int h= 68*StepUnit; //34 * StepUnit + ceiling;
int ceiling = h / 4; //5; // 10*StepUnit;
int margin = w / 4; //4;

// Coordinates of current (starting) point
int x1= w/2;
int y1= h/2;

// Approximate length of strings from marker to staple
int a1= sqrt(pow(x1,2)+pow(y1,2));
int b1= sqrt(pow((w-x1),2)+pow(y1,2));

//globals

unsigned long lastTime;
int penPos, lastPenPos;
boolean draw= false;
boolean stepping = false;
int drawCount = 0;

#define STATUS_LED 4                                    
#define OPTO_ROLLER A4
#define OPTO_PEN A5
#define STEP_PWM 9 

#define XBEETX 2
#define XBEERX 3



//pwm is causing arduino to reboot at low values - check with scope
int PWM_LOW = 1; 
int PWM_HIGH = 55;
#define delayFactor 1 //10 //when we change pwmFrequency, delays change in value so multiply by this
#define PWM_CHANGE_DELAY 1 * delayFactor

#define DEBUG

NewSoftSerial xbeeSerial(XBEERX, XBEETX);
#ifdef XBEE

TimedAction ActionCheckXbeeData = TimedAction( 1000, checkXbeeData);
#endif
#ifdef DEBUG
TimedAction ActionCheckSerialData = TimedAction( 200, checkSerialData);
#endif

TimedAction ActionTurnOffSteppers = TimedAction( 500, turnOffSteppers );
//TimedAction ActionDraw = TimedAction( 1000, draw );
//TimedAction ActionLEDColour = TimedAction( 1000, LEDColour );
// initialize the stepper library on pins 8 through 11:
// 3 brown
// 4 red
// 5 white
// 6 green

Stepper leftStepper(stepsPerRevolution, A0,A1,A2,A3);            
Stepper rightStepper(stepsPerRevolution, 13,10,11,12);            

void setup() {
  pinMode( STATUS_LED, OUTPUT );
  pinMode( STEP_PWM, OUTPUT );
  pinMode(OPTO_ROLLER,INPUT);
  pinMode(OPTO_PEN,INPUT);
  //digitalWrite(OPTO_ROLLER,LOW);
 // digitalWrite(OPTO_PEN,LOW);
  // set the speed at 60 rpm:
  leftStepper.setSpeed(stepSpeed / delayFactor);
  rightStepper.setSpeed(stepSpeed / delayFactor);
  
  setPwmFrequency( STEP_PWM, 1 ); //set to 32khz / 1
  
  analogWrite( STEP_PWM, PWM_HIGH );
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println( "energy plotter startup" );  
 // #ifdef XBEE
  xbeeSetup();
 // #endif

      digitalWrite( STATUS_LED, HIGH );
      delay(1000 * delayFactor);
      digitalWrite( STATUS_LED, LOW );



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
        drawEnergy( energy, minute );
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
  
  if( Serial.available() )
  {
        digitalWrite( STATUS_LED, HIGH );
    char command = Serial.read();
    switch( command )
    {
      case 'b':
        burnTest( serReadInt() );
        break;
      case 'e':
      {
        int energy = serReadInt();
        int minute = serReadInt();
        drawEnergy( energy, minute );
        Serial.print( "set energy to: " );
        Serial.print( energy );
        Serial.print( " at " );
        Serial.println( minute );
        break;
      }
      case 'w':
//        pwm = serReadInt();
        PWM_LOW = serReadInt();
        Serial.print( "set pwm_low: " );
        Serial.println( PWM_LOW );
        break;
      case 'h':
        PWM_HIGH = serReadInt();
        Serial.print("set pwn high: ");
        Serial.println( PWM_HIGH );
        break;
      case 'l':
        step( LEFT, serReadInt() );
        break;
      case 'r':
         step( RIGHT, serReadInt() );
         break;
      case 'v': //draw a straight line
      {
        int x = serReadInt();
        x *= StepUnit;
        int y = serReadInt();
        y *= StepUnit;
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
        digitalWrite( STATUS_LED, LOW );
  }
}
#endif
