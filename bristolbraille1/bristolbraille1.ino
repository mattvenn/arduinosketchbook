#define STEPS 100
#include <Stepper.h>
Stepper stepper(STEPS, 2,3,4,5);
#define LED 13
#define SOLENOID1 6
#define SOLENOID2 8
#define SOLENOID3 7



void setup()
{
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(40);
  pinMode(LED,OUTPUT);
  pinMode(SOLENOID1,OUTPUT);
  pinMode(SOLENOID2,OUTPUT);
  pinMode(SOLENOID3,OUTPUT);

}

void loop()
{

  stepper.step(-10);
  stepper.step(10);

  flash(LED,100);
  stepper.step(100);

  int sol1 = random(0,100);
  int sol2 = random(0,100);
  int sol3 = random(0,100);

  for( int i =0; i < 100; i ++ )
  {
    stepper.step(1);
    if( i == sol1 )
      digitalWrite(SOLENOID1,HIGH);
    if( i == sol2 )
      digitalWrite(SOLENOID2,HIGH);
    if( i == sol3 )
      digitalWrite(SOLENOID3,HIGH);
  }
  stepper.step(100);

  digitalWrite(SOLENOID1,LOW);
  digitalWrite(SOLENOID2,LOW);
  digitalWrite(SOLENOID3,LOW);



  delay(2000);


}

void flash(int pin, int delayT)
{
  digitalWrite(pin,HIGH);
  delay(delayT);
  digitalWrite(pin,LOW);
}


