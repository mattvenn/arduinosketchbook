
#define STEPPER

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

#include <MsTimer2.h>       
// for your motor
#ifdef STEPPER
// initialize the stepper library on pins 8 through 11:
#include <Stepper.h>
#define PWM_LOW 10
#define PWM_HIGH 100
Stepper leftStepper(stepsPerRevolution, A5,A4,A3,A2);       
Stepper rightStepper(stepsPerRevolution, A1,A0,8,7);       
#endif

#define SERVO

#ifdef SERVO
#include <Servo.h> 
Servo myservo;  // create servo object to control a servo 
#endif                // a maximum of eight servo objects can be created 


void setPwmFrequency(int pin, int divisor);
#define RADIO
#ifdef RADIO  
//for radio
#include <JeeLib.h>

MilliTimer sendTimer;
byte needToSend;

#endif
#define SERVO_PIN 9
#define STEP_PWM 3 

#define LED 4

//payload def
typedef struct {
  int servo;
  int stepperL;
  int stepperR;

} 
Payload;

Payload payload;

void setup()
{
  Serial.begin(57600);
  Serial.println( "receiver started" );
  setPwmFrequency( STEP_PWM, 1 ); //set to 32khz / 1
  analogWrite( STEP_PWM, PWM_LOW );
  pinMode(LED,OUTPUT);
#ifdef STEPPER
  leftStepper.setSpeed(20);
  rightStepper.setSpeed(20);
#endif

#ifdef SERVO
  myservo.attach(SERVO_PIN);
#endif


#ifdef RADIO
  Serial.println( "initialising radio" );
  delay(100);
  rf12_initialize(1, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
#endif
  Serial.println( "steup done");
}

byte pos = 0;
void loop()
{
  if( Serial.available() )
  {
    //need to delay for serreadint to work
    delay(100);
    digitalWrite( LED, HIGH );
    char command = Serial.read();
    switch( command )
    {
    case 's':
      {
        Serial.println("moving servo" );
        myservo.write( serReadInt());
      }
    case 'p':
      analogWrite( STEP_PWM, serReadInt() );
      break;           
    case 'l':
      leftStepper.step(  serReadInt());

      break;
    case 'r':
      rightStepper.step(  serReadInt());

      break;
    }
    digitalWrite( LED, LOW );
  }
#ifdef RADIO
  if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    Serial.print( "got data: " );
    Serial.println( p->servo );
    Serial.println( p->stepperL );
    Serial.println( p->stepperR );
    if( p->servo != 0 )
    {
      myservo.write(p->servo );

    }

    if( p->stepperL != 0 )
    {
      analogWrite( STEP_PWM, PWM_HIGH );           
      leftStepper.step(p->stepperL);
      analogWrite( STEP_PWM, PWM_LOW );
      Serial.println( "pwm is low" );
    }
    if( p->stepperR != 0 )
    {
      analogWrite( STEP_PWM, PWM_HIGH );           
      rightStepper.step(p->stepperR);
      analogWrite( STEP_PWM, PWM_LOW );
      Serial.println( "pwm is low" );
    }


  }
#endif

  //send data once 
#ifdef RADIO
  if (sendTimer.poll(500))
  {

    //  needToSend = 1;

    //  Serial.println( "data ready to send:" );


  }
#endif

#ifdef RADIO
  if (needToSend && rf12_canSend())
  {

    needToSend = 0;
    //broadcast
    rf12_sendStart(0, &payload, sizeof payload);
    Serial.println("sent");

  }
#endif
}



