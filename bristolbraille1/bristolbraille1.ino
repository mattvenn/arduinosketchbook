//setup for stepper motor (our motor is 3.6degree)
#define STEPS 100
#include <Stepper.h>
Stepper stepper(STEPS, 2,3,4,5);

//led
#define LED 13

//define solenoid pins
#define SOLENOID1 6
#define SOLENOID2 8
#define SOLENOID3 7

//what to do at the start
void setup()
{
  // set the speed of the motor (RPMs)
  stepper.setSpeed(40);
  
  //pin modes of the control pins
  pinMode(LED,OUTPUT);
  pinMode(SOLENOID1,OUTPUT);
  pinMode(SOLENOID2,OUTPUT);
  pinMode(SOLENOID3,OUTPUT);
}

//keep doing this
void loop()
{
  //reverse by 10 steps to free the sliders
  stepper.step(-10);
  stepper.step(10);

  flash(LED,100);
  
  //random trigger points for solenoids
  int sol1 = random(0,100);
  int sol2 = random(0,100);
  int sol3 = random(0,100);

  //during a whole turn, fire the solenoids on their random trigger point
  for( int stepNum =0; stepNum < 100; stepNum ++ )
  {
    //take one step on the motor
    stepper.step(1);
    
    //check if we need to turn on the solenoid
    if( stepNum == sol1 )
      digitalWrite(SOLENOID1,HIGH);
    if( stepNum == sol2 )
      digitalWrite(SOLENOID2,HIGH);
    if( stepNum == sol3 )
      digitalWrite(SOLENOID3,HIGH);
  }
  
  //another full cycle
  stepper.step(100);

  //turn off the solenoids
  digitalWrite(SOLENOID1,LOW);
  digitalWrite(SOLENOID2,LOW);
  digitalWrite(SOLENOID3,LOW);

  //wait for next cycle
  delay(2000);
}

void flash(int pin, int delayT)
{
  digitalWrite(pin,HIGH);
  delay(delayT);
  digitalWrite(pin,LOW);
}


