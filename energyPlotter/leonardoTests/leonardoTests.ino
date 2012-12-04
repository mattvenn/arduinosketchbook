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

todo:
  needs radio and sd card interferance fixing
  needs to resolve the possibility of gondola and pen state getting mixed up. This happens in the gondola code. it needs a timeout to set it back to listening mode not timing mode.

*/
#define testSteppers
//#define useSD //uses 10k
#define useRadio //uses 180bytes?!
#define testLED
#define testServo
//#define testIO
//#define testMem
#include <EEPROM.h>
#include <SPI.h>
#include <JeeLib.h>

MilliTimer statusTimer,sdTimer;

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
long int lastCommandTime = 0;

//drawing globals
// Approximate dimensions (in steps) of the total drawing area
#define stepsPerRevolution 200
const float DIAMETER = 1.05; //for thin green wire 1.01; //for thin stainless
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
int idAddress = 0;
byte id;

//payload def
typedef struct {
 // byte index;
  char id;
  char command;
  int arg1;
  int arg2;

} Payload;
Payload payload;

boolean penState = 0;

void setup() {
  Serial.begin(9600);
  
  // EEPROM.write(idAddress,2); //set address
  id = getId();
  Serial.print("started, robot id:");
  Serial.println(id));
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
  initServo();
  //leave some time in case this doesn't work. Makes it easier to reprogram!  
  //delay(4000);
  #ifdef useSD
    initSD();
  #endif
  #ifdef useRadio
  initRadio();
  checkRadio = true;    
  #endif
  
  //calibrate();
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
    //try to cope with lost packets. Send an ack if enough time has elapsed since we last completed a command.
  /*  if( millis() - lastCommandTime > 10000 ) 
    {
      sendAck = 1;
      Serial.println( millis() );
    }*/
  }

  if( testSD && sdTimer.poll(5000) )
  {
 //   readSD();
  //  writeSD(i++);
  }
   

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
 //   stopRadio();
    commandWaiting = false;
    switch( payload.command )
    {
      case 't':
        servoTest = payload.arg1; 
        Serial.print( "line tset: " ); Serial.println( servoTest );
        break;
      case 'y':
       digitalWrite( SERVO, payload.arg1 ? PENDOWN : PENUP );
        Serial.println("ok");
         break;   
      case 'd':
     
       // if( payload.arg1 != penState )
        {
          payload.arg1 ? penUp() : penDown();
          Serial.println( payload.arg1 ? "pen down" : "pen up" );
          Serial.println( "ok");
          penState = payload.arg1;
        }  
       /* else
        {
           Serial.print( "pen already: ");
          Serial.println( penState ? "down" : "up" );
        }*/
        break;
      
   
      case 'c':
        penUp();
        calibrate();
        //then need to move 60,-80
        stepLeft(60);
        stepRight(-80);
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
         Serial.print( "x(cm): " );
         Serial.print( x1 / StepUnit );
         Serial.print( " y(cm): " );
         Serial.println( y1 / StepUnit);

         Serial.print( "x: " );
         Serial.print( x1  );
         Serial.print( " y): " );
         Serial.println( y1 );


         Serial.print( " a1: " );
         Serial.print( a1 / StepUnit);
         Serial.print( " b1: " );
         Serial.println( b1 / StepUnit);
         Serial.print( "stepunit: " );
         Serial.println( StepUnit );
         Serial.print( "motordist: " );
         Serial.println( MOTOR_DIST_CM);
         Serial.println("ok");
         payload.arg1 = x1 / StepUnit;
         payload.arg2 = y1 / StepUnit;
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
        readSD();
        //#ifdef useRadio
        //initRadio(); //need this after a write/read some combo?
        //#endif
        break;  
      #endif
      #ifdef useRadio
      case 'r':
        initRadio();
        Serial.println( "useradio" );
     //   readSD();
        break;
      case 'x':
        detachInterrupt(0);
        Serial.println( "detach inter" );
        SPI.end();
        break;
      #endif
      default:
        Serial.println( "bad command");
        break;
    }
//    startRadio();
//    initRadio();
     sendAck = true;   
     lastCommandTime = millis();
   //  Serial.print( "command finished at: " ); // putting these in will break the feeder 
 //    Serial.println( lastCommandTime ); //putting these in will break the feeder
  }
#ifdef useRadio        
if( checkRadio)
  doRadio();
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
