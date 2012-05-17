#include <AccelStepper.h>
#define DIRL A3
#define STEPL A4
#define DIRR A1
#define STEPR A2
#define PWMPIN 3
#define LED 4

#define PWM_HIGH 80
#define PWM_LOW 20
const int dtime = 3;
#define SERVO_PIN 9
#include <Servo.h> 
Servo myservo;  

void forwarda() {  
    digitalWrite( DIRL, LOW );
      digitalWrite( STEPL, HIGH );
      delay(2);
        digitalWrite( STEPL, LOW );
              delay(2);
//  leftStepper.step(-1);

}
void backwarda() {  
    digitalWrite( DIRL, HIGH );
      digitalWrite( STEPL, HIGH );
      delay(2);
        digitalWrite( STEPL, LOW );
              delay(2);
//  leftStepper.step(1);
}

AccelStepper stepper1(forwarda, backwarda);


void forwardb() {  
    digitalWrite( DIRR, LOW );
      digitalWrite( STEPR, HIGH );
  //   / delay(2);
        digitalWrite( STEPR, LOW );
         //     delay(2);

}
void backwardb() {  
    digitalWrite( DIRR, HIGH );
      digitalWrite( STEPR, HIGH );
   //   delay(2);
        digitalWrite( STEPR, LOW );
           //   delay(2);

}

AccelStepper stepper2(forwardb, backwardb);
  
void setup()
{
    myservo.attach(SERVO_PIN);
  pinMode( LED, OUTPUT );
  pinMode( DIRR, OUTPUT );
  pinMode( DIRL, OUTPUT );
  pinMode(STEPR, OUTPUT );
  pinMode(STEPL, OUTPUT );
  analogWrite( PWMPIN, PWM_HIGH );
   stepper1.setMaxSpeed(200.0);
    stepper1.setAcceleration(100.0);
    stepper1.moveTo(100);
    
    stepper2.setMaxSpeed(200.0);
    stepper2.setAcceleration(100.0);
    stepper2.moveTo(150);
}
void loop()
{
/*  if( stepper1.distanceToGo() == 0 )
    stepper1.moveTo( -stepper1.currentPosition() );
      if( stepper2.distanceToGo() == 0 )
    stepper2.moveTo( -stepper2.currentPosition() );
    stepper1.run();
    stepper2.run();
  */  
  digitalWrite( LED, HIGH );
analogWrite( PWMPIN, PWM_HIGH );
  digitalWrite( DIRL, LOW );
  digitalWrite( DIRR, LOW );
  steps(100);

  digitalWrite( DIRL, HIGH );
  digitalWrite( DIRR, HIGH );
  steps(100);
  analogWrite( PWMPIN, PWM_LOW );
    digitalWrite(LED, LOW );
            myservo.write(180 );
  delay(2000);
        myservo.write(0 );
  
}

void steps( int num )
{
  for( int step = 0; step < num ; step ++ )
  {
    digitalWrite( STEPL, HIGH );
        digitalWrite( STEPR, HIGH );
    delay( dtime );
    digitalWrite( STEPL, LOW );
        digitalWrite( STEPR, LOW );
    delay( dtime );
  }
}
