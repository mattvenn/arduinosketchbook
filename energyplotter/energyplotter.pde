
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
#define stepsPerRevolution 100  // change this to fit the number of steps per revolution
#define LOOP_PERIOD 400.0 //seconds
#define MAX_PEN_STEPS 2000
#define MAX_ENERGY 4000 //W

//globals
float millisPerStep;
unsigned long lastTime;
int energy;
int penPos, lastPenPos;


#define STATUS_LED 4                                    
#define OPTO_ROLLER A4
#define OPTO_PEN A5
#define PWM_RED 3 //todo
#define PWM_GREEN 4 //todo
#define XBEETX 5
#define XBEERX 6




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
Stepper penStepper(stepsPerRevolution, A0,A1,A2,A3);            
Stepper rollerStepper(stepsPerRevolution, 9,10,11,12);            

void setup() {
  pinMode( STATUS_LED, OUTPUT );
  pinMode( PWM_RED, OUTPUT );
  pinMode( PWM_GREEN, OUTPUT );
  pinMode(OPTO_ROLLER,INPUT);
  pinMode(OPTO_PEN,INPUT);
  //digitalWrite(OPTO_ROLLER,LOW);
 // digitalWrite(OPTO_PEN,LOW);
  // set the speed at 60 rpm:
  penStepper.setSpeed(60);
  rollerStepper.setSpeed(60);
  
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println( "energy plotter startup" );  
  xbeeSetup();
  calibrate();
}


void loop()
{
  #ifdef DEBUG
  ActionCheckSerialData.check();
  #endif
  ActionCheckXbeeData.check();
  ActionStatusBlink.check();
  //ActionDraw.check();
  if( millis() - lastTime > millisPerStep )
  {
    lastTime = millis();
    draw();
  }

}

void LEDColour()
{
  analogWrite( PWM_RED,   map( energy, 0, 1024, 0, 255 ) );
  analogWrite( PWM_GREEN, map( energy, 1024, 0, 0, 255 ) ); 
}

void draw()
{
  //advance roller and pen (at same time?)
  rollerStepper.step(1);
  
  penPos = map( energy, 0, MAX_ENERGY, 0, MAX_PEN_STEPS );
  if( penPos > MAX_PEN_STEPS )
    penPos = MAX_PEN_STEPS;
  
  penStepper.step(penPos - lastPenPos);
  lastPenPos = penPos;  
}

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
    rollerStepper.step(1);
  Serial.println( "found hole, winding again...");
  int count = 0;
  //ignore opto until we've moved the paper on a bit
  while( limit( OPTO_ROLLER ) )
  {
    rollerStepper.step(1);
    count ++;
  }
  while( ! limit( OPTO_ROLLER ) )
  {
    count ++;
    rollerStepper.step(1);
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
   penStepper.step(-1);
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
      case 'c':
        calibrate();
        break;
    }
  }
}
#endif
