#include <Servo.h>
#define SERVO A3
#include <JeeLib.h>


#define DIRR 0
#define DIRL 4

#define LIMITL 13
#define LIMITR 5


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
#include <AccelStepper.h>

AccelStepper stepperL(fsL, bsL); // use functions to step
AccelStepper stepperR(fsR, bsR); // use functions to step

const float maxSpeed = 600.0;
const int pwmVal = 130;
const int stepTime = 2;
void fsL()
{
  digitalWrite( DIRL, LOW );
  digitalWrite( STEPL, LOW );
  delay(stepTime);
  digitalWrite( STEPL, HIGH );
  delay(stepTime);
}
void bsL()
{
  digitalWrite( DIRL, HIGH );
  digitalWrite( STEPL, LOW );
  delay(stepTime);
  digitalWrite( STEPL, HIGH );
  delay(stepTime);
}
void fsR()
{
  digitalWrite( DIRR, LOW );
  digitalWrite( STEPR, LOW );
  delay(stepTime);
  digitalWrite( STEPR, HIGH );
  delay(stepTime);
}
void bsR()
{
  digitalWrite( DIRR, HIGH );
  digitalWrite( STEPR, LOW );
  delay(stepTime);
  digitalWrite( STEPR, HIGH );
  delay(stepTime);
}

//#define RADIO
MilliTimer sendTimer,statusTimer;
byte needToSend;
boolean ledState = false;
//payload def
typedef struct {
  byte servo;
  int stepperL;
  int stepperR;

} 
Payload;

Payload payload;

//Servo myServo;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  Serial.println("started");
 // myServo.attach(SERVO);  
  // initialize the digital pin as an output.
  pinMode(SERVO,OUTPUT);
  pinMode(led, OUTPUT);   
  pinMode(MS1, OUTPUT );
  pinMode(MS2, OUTPUT );
  //pinMode( GPIO1, OUTPUT);
  //pinMode(GPIO2, OUTPUT );
//  pinMode( GPIO1, INPUT );
//  pinMode( GPIO2, INPUT );
  pinMode( SD_SEL, OUTPUT );
  pinMode( RFM_SEL, OUTPUT );
  pinMode( PWML, OUTPUT );
  pinMode( PWMR, OUTPUT );
  
  pinMode( DIRL, OUTPUT );
  pinMode( DIRR, OUTPUT );
  pinMode( STEPL, OUTPUT );
  pinMode( STEPR, OUTPUT );
  pinMode( LIMITL, INPUT );
  digitalWrite( LIMITL, HIGH );
  pinMode( LIMITR, INPUT );
  digitalWrite( LIMITR, HIGH );
digitalWrite(MS1, LOW ); //both low is no microstep 
digitalWrite(MS2, LOW);

//spi test
pinMode(SCK,OUTPUT);
pinMode(MOSI,OUTPUT);
pinMode(MISO,OUTPUT);



  digitalWrite(led,HIGH);
 attachInterrupt(0, blink, RISING);
 analogWrite(PWMR, pwmVal );
 analogWrite(PWML, pwmVal );

 stepperL.setMaxSpeed(maxSpeed);
    stepperL.setAcceleration(200.0);
    stepperL.moveTo(400);
    
    stepperR.setMaxSpeed(maxSpeed);
    stepperR.setAcceleration(200.0);
    stepperR.moveTo(300);
    
  delay(4000);
    #ifdef RADIO
  rf12_initialize(1, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
  #endif
  
}

volatile int b = 0;
void blink()
{
  b ++;
}

// the loop routine runs over and over again forever:
void loop() {
  
   if( statusTimer.poll(500) )
   {
    // Serial.println( "poll");
     ledState = ! ledState;
     digitalWrite(led,ledState);
    
    digitalWrite(SCK,ledState);
        digitalWrite(MOSI,ledState);
            digitalWrite(MISO,ledState);
   }
/*  Serial.println( "step");
  for ( int i = 0; i < 100 ; i ++ )
  {
forwardstep();
delay(10);
  }
  for ( int i = 0; i < 100 ; i ++ )
  {
backwardstep();
delay(10);
  }*/
  /*
    if (stepperL.distanceToGo() == 0)
	stepperL.moveTo(-stepperL.currentPosition());
    stepperL.run();
    
     if (stepperR.distanceToGo() == 0)
	stepperR.moveTo(-stepperR.currentPosition());
    stepperR.run();
   */ 
  
/* 
  //lift servo
  pulsePower( 1, 20 );
  delay(2000);
    pulsePower( 1, 600 );
 */
#ifdef RADIO
  //doRadio();
  #endif
}
/*
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


  myServo.write( 180 );
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
  delay(1000);               // wait for a second
}
*/


void pulsePower( int pulseLength, int delayTime )
{
    Serial.print( "pulsing servo line: " );
    Serial.println( delayTime );
    setPowerPin(LOW);
    delay(2000);
      setPowerPin( HIGH   );
      delay( pulseLength );
      setPowerPin( LOW );
      delay( delayTime );
            setPowerPin( HIGH   );
            delay( pulseLength );
      setPowerPin( LOW );
      //wait for servo to move
      delay(2000);
    setPowerPin(HIGH);
  
}
void setPowerPin( boolean state )
{
    digitalWrite(SERVO,  state );
  digitalWrite(led, ! state );
}

