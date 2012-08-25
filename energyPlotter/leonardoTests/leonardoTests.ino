/* 
total program size is 19k with sd and radio, 9k without sd
grbl is about 17k. Available is 28k with bootloader.

strange spi problems where radio stopped sending after an sd write has stopped.
Just discovered this is because the spi programmer was still plugged in. 
Doesn't need to be on, just attached. 
So must be an electrical characteristic of the spi bus is wrong 
eg pullup/down resistors required. a pulldown of 1k on miso seems to solve the problem.
setting miso to pullup prevents radio from working but sd still works.
check with a scope

jeelib/rf12.cpp needs adjusting to set rf12 chip select to portf bit 0

*/
#define testSteppers
//#define useSD //uses 10k
#define useRadio //uses 180bytes?!
#define testLED
#define testServo
//#define testIO
//#define testMem

#include <SPI.h>
#include <JeeLib.h>

#ifdef useRadio
MilliTimer statusTimer,sdTimer;
#endif

#include <AccelStepper.h>

//servo constants
#define PENDOWN 300
#define PENUP 450
#define PULSELEN 2

//pin defs
#define DIRR 0
#define DIRL 4

#define LIMITL 13
#define LIMITR 5
#define SERVO A3
#define STEPR 2
#define STEPL 1
#define GPIO1 6
#define led 7
#define MS1 8
#define MS2 9
#define PWML 10
#define PWMR 11

#define GPIO2 12

#define GPIO1_IN A7
#define GPIO2_IN A11
#define SD_SEL A4
#define RFM_SEL A5
#define RFM_INT 3

boolean commandWaiting = false;
boolean sendAck = false;
int servoPos = 20;
boolean ledState = false;
boolean servoState = false;
boolean testSD = false; //auto test the SD
boolean checkRadio = false;
boolean servoTest = false;
int i = 0;

//drawing globals
// Approximate dimensions (in steps) of the total drawing area
#define stepsPerRevolution 200
const float DIAMETER = 1.01; //for thin stainless
const float circumference = 3.1415 * DIAMETER;
 float StepUnit = stepsPerRevolution / circumference;   
float MOTOR_DIST_CM = 64;
float w= MOTOR_DIST_CM*StepUnit;
float h= MOTOR_DIST_CM*StepUnit; 
int x1 = w/2;
int y1 = 20 * MOTOR_DIST_CM;
//int x2,y2; 
// Approximate length of strings from marker to staple
int a1= sqrt(pow(x1,2)+pow(y1,2));
int b1= sqrt(pow((w-x1),2)+pow(y1,2));

//payload def
typedef struct {
  char command;
  int arg1;
  int arg2;

} Payload;
Payload payload;

void setup() {
  Serial.begin(9600);
  Serial.println("started");
  
  pinMode(led, OUTPUT);   
  digitalWrite(led,HIGH);

  // initialize the digital pin as an output.
  pinMode(SERVO,OUTPUT);
  setPowerPin(LOW);

  //stepper microstep control pins  
  pinMode(MS1, OUTPUT );
  pinMode(MS2, OUTPUT );
  pinMode( PWML, OUTPUT );
  pinMode( PWMR, OUTPUT );
  //stepper step and dir
  pinMode( DIRL, OUTPUT );
  pinMode( DIRR, OUTPUT );
  pinMode( STEPL, OUTPUT );
  pinMode( STEPR, OUTPUT );

  //pinMode( GPIO1, OUTPUT);
  //pinMode(GPIO2, OUTPUT );
  //  pinMode( GPIO1, INPUT );
  //  pinMode( GPIO2, INPUT );

  //spi setup
  pinMode( SS, OUTPUT ); //needed to make us the master of spi bus
  pinMode( SD_SEL, OUTPUT );
  pinMode( RFM_SEL, OUTPUT );
  digitalWrite( SD_SEL, HIGH );
  digitalWrite(RFM_SEL, HIGH); 
  
  //limits
  pinMode( LIMITL, INPUT );
  digitalWrite( LIMITL, HIGH );
  pinMode( LIMITR, INPUT );
  digitalWrite( LIMITR, HIGH );

  //config steppers
  initSteppers();
  //leave some time in case this doesn't work. Makes it easier to reprogram!  
  //delay(4000);
  #ifdef useSD
    initSD();
  #endif
  #ifdef useRadio
 // initRadio();
 // checkRadio = true;    
  #endif
}

// the loop routine runs over and over again forever:
void loop() {


  if( statusTimer.poll(500) )
  {
    #ifdef testLED
    //Serial.println( "led");
    ledState = ! ledState;
    digitalWrite(led,ledState);
    #endif
    #ifdef testMem
    Serial.print("mem:");
    Serial.println(freeMemory());
    #endif
    //sendAck = 1;
  }

  if( testSD && sdTimer.poll(5000) )
  {
 //   readSD();
  //  writeSD(i++);
  }
  /*
  if( servoTest )
  {
     setMS(HIGH,LOW);
     setSpeed(1000);
     setAccel(1000);
     pulsePower( PULSELEN, PENUP ); 
    moveSteppers(0,0);
    for( int i = 100; i < 800; i += 10 )
    {
     //start pos
      moveSteppers(i,i); 
      delay(1000); //otherwise servo micro getss confused
      pulsePower( PULSELEN, PENDOWN);
      moveSteppers(-800,800);
      pulsePower( PULSELEN, PENUP ); 
      moveSteppers(800,-800);
      pulsePower( PULSELEN, PENDOWN ); 
      moveSteppers(i,i); 
      pulsePower( PULSELEN, PENUP ); 
    }
    servoTest = false;
  }
  */

  if(Serial.available() > 0 )
  {
    payload.command = Serial.read();
    payload.arg1 = serReadInt();
    payload.arg2 = serReadInt();
    commandWaiting = true;
    Serial.flush();
  }

  if( commandWaiting )
  {
    commandWaiting = false;
    switch( payload.command )
    {
      case 't':
        servoTest = payload.arg1; 
        break;
      case 'd':
        pulsePower( PULSELEN, payload.arg1 ? PENDOWN : PENUP );
        Serial.println( payload.arg1 ? "pen down" : "pen up" );
        Serial.println( "ok");
        break;
      case 's':
        pulsePower( payload.arg2, payload.arg1 );
        Serial.println("ok");
        break;
      case 'c':
        pulsePower( PULSELEN, PENUP ); 
        calibrate();
        Serial.println("ok");
        break;
      case 'a':
        setAccel(payload.arg1);
        break;
      case 'm':
        stepLeft(payload.arg1);
        stepRight(payload.arg2);
        
        break;
      case 'g':
          drawLine(x1,y1,payload.arg1,payload.arg2);
          Serial.println( "ok" );
        break;
      case 'q':
         Serial.println( "pos" );
         Serial.print( "x: " );
         Serial.print( x1 / StepUnit );
         Serial.print( " y: " );
         Serial.println( y1 / StepUnit);
         Serial.print( " a1: " );
         Serial.print( a1 / StepUnit);
         Serial.print( " b1: " );
         Serial.println( b1 / StepUnit);
         Serial.print( "stepunit: " );
         Serial.println( StepUnit );
         Serial.print( "motordist: " );
         Serial.println( MOTOR_DIST_CM);
         Serial.println("ok");
         break;
      case 'p':
        setSpeed(payload.arg1);
        setPWM(payload.arg2);
        Serial.println("ok");
      break;
      case 'i':
        setMS(payload.arg1,payload.arg2);
        Serial.println("ok");
      break;
      #ifdef useSD
      case 'w':
        writeSD(payload.arg1);
        
        //#ifdef useRadio
        //initRadio(); //need this after a write/read some combo?
        //#endif
        break;  
      #endif
      #ifdef useRadio
      case 'r':
     //   readSD();
        break;
      #endif
      default:
        Serial.println( "bad command");
        break;
    }
     sendAck = true;   
  }
#ifdef useRadio        
//if( checkRadio)
 // doRadio();
#endif

#ifdef testIO
  Serial.print( "ints: " );
  Serial.println( b );
  Serial.print( "gpio1:" );

  Serial.println( analogRead( GPIO1_IN ) );
  Serial.print( "gpio2:" );
  Serial.println( analogRead( GPIO2_IN ) );
  myServo.write( 0 );
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(MS1, HIGH );
  digitalWrite(MS2, HIGH );
  //  digitalWrite( GPIO1, HIGH);
  //  digitalWrite(GPIO2, HIGH );
  digitalWrite( SD_SEL, HIGH );
  digitalWrite( RFM_SEL, HIGH );

  digitalWrite( STEPL, HIGH );
  digitalWrite( STEPR, HIGH );
  digitalWrite( DIRL, HIGH );
  digitalWrite( DIRR, HIGH );

  Serial.println( digitalRead(LIMITL) ? "LIMITL 1" : "LIMITL 0" );
  Serial.println( digitalRead(LIMITR) ? "LIMITR 1" : "LIMITR 0" );  
  analogWrite(PWML, 100 );
  analogWrite(PWMR, 100 );

  delay(1000);       

  digitalWrite( STEPL, LOW );
  digitalWrite( STEPR, LOW );
  digitalWrite( DIRL, LOW );
  digitalWrite( DIRR, LOW );

  // wait for a second
  digitalWrite(led, LOW);
  digitalWrite(MS1, LOW );
  digitalWrite(MS2, LOW );
  //  digitalWrite( GPIO1, LOW);
  //  digitalWrite(GPIO2, LOW );
  digitalWrite( SD_SEL, LOW );
  digitalWrite( RFM_SEL, LOW );
  analogWrite(PWML, 10 );
  analogWrite(PWMR, 10 );
  // turn the LED off by making the voltage LOW

#endif
}
