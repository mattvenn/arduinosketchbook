#ifdef testSteppers


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#define BIT_TST(REG, bit, val) ( ( (REG & (1UL << (bit) ) ) == ( (val) << (bit) ) ) )

const int HOME_PWM_HIGH = 100;
const int HOME_PWM_LOW = 10;
const int HOME_SPEED = 4;
const int maxSpeed = 3; //800;
const int DEFAULT_PWM = 60;
//const int steps = 200;
int stepTime = 2;
const int SUPERFAST_ACCELERATION = 6000;
AccelStepper leftStepper(fsL,bsL);
AccelStepper rightStepper(fsR,bsR);

#include <digitalWriteFast.h>

void initSteppers()
{
  setPWM(DEFAULT_PWM);
  setSpeed(maxSpeed);
  setAccel(SUPERFAST_ACCELERATION);

  /* micro stepping stuff
   LOW, LOW = no micro stepping : works
   HIGH, LOW = half step : works
   LOW, HIGH = quarter : works
   high, high = eigth step :works
   */
  setMS(LOW,LOW); //both low is no microstep 

}

void setPWM(int pwm)
{
  setPWML(pwm);
  setPWMR(pwm);
}
void setPWML(int pwm)
{
  analogWrite(PWML, pwm);
  Serial.print( "pwmL = ");
  Serial.println( pwm);
}
void setPWMR(int pwm)
{
  analogWrite(PWMR, pwm);
  Serial.print( "pwmR = ");
  Serial.println( pwm);
}

void calibrate()
{
  setMS(LOW,LOW);
  setSpeed( HOME_SPEED );
  findLeftLimit();
  //for now, pay out left side
//  stepLeft(1000);

  int steps = findRightLimit();
  Serial.print( "steps to right: " ); Serial.println( steps );
  const int GONDOLAWIDTH = 14;
  /*
  MOTOR_DIST_CM = steps / StepUnit;
  MOTOR_DIST_CM += GONDOLAWIDTH ;
  */
  MOTOR_DIST_CM - 64;
  Serial.print( "width (cm): " ); Serial.println( MOTOR_DIST_CM);
  w= MOTOR_DIST_CM*StepUnit;
  h= 0;
  Serial.print( "w = ");
  Serial.println( w );
  x1 = w - (GONDOLAWIDTH/2) * StepUnit;
  y1 = 1 * StepUnit; //FIXME;
  a1= sqrt(pow(x1,2)+pow(y1,2));
  b1= sqrt(pow((w-x1),2)+pow(y1,2));
  Serial.print( "a1:" ); Serial.println( a1);
  Serial.print( "b1:" ); Serial.println( b1 );
  Serial.println( "about to move to midpoint" );
  setPWM(DEFAULT_PWM);

  drawLine(x1,y1,w/2,w/2);
}

int findRightLimit()
{
  int steps = 0;

  //set pwmR to high
  setPWMR(HOME_PWM_HIGH);
  //set pwmR to low
  setPWML(HOME_PWM_HIGH);

  //while limit l is high, wind l motor
  while( digitalRead( LIMITR ) == HIGH )
//  while( bit_is_set( PORTC, 7 ) )  //reg bit val
  {
    stepLeft(+1);
    stepRight(-1);
    steps++;
  }
  Serial.println( "right limit reached" );
  return steps;
}

int findLeftLimit()
{
  int steps = 0;

  //set pwmL to high
  setPWML(HOME_PWM_HIGH);
  //set pwmR to low
  setPWMR(HOME_PWM_LOW);

  //while limit l is high, wind l motor
  while( digitalRead( LIMITL ) == HIGH )
//  while( bit_is_set( PORTC, 6 ) ) 
  {
    stepLeft(-1);
    stepRight(-1);
    steps ++;
//    steps +=5;
  }
  Serial.println( "left limit reached" );
  return steps;
}

void setMS(int ms1, int ms2)
{
  digitalWrite(MS1, ms1 ); //both low is no microstep 
  digitalWrite(MS2, ms2);
  Serial.print( "ms1 set to" );
  Serial.println(ms1);
  Serial.print( "ms2 set to" );
  Serial.println(ms2);
}

void setSpeed(int speed)
{
  //leftStepper.setMaxSpeed(speed);
  //rightStepper.setMaxSpeed(speed);
  stepTime = speed;
  Serial.print( "speed set to" );
  Serial.println(speed);
}
void setAccel(int accel)
{
  leftStepper.setAcceleration(accel);
  rightStepper.setAcceleration(accel);
  Serial.print( "accel set to" );
  Serial.println(accel);
}

/*void moveSteppers( int posL, int posR)
{

  Serial.print( "step");
  Serial.print( posL );
  Serial.print( ",");
  Serial.println( posR );
  leftStepper.moveTo(leftStepper.currentPosition() + posL);
  rightStepper.moveTo(rightStepper.currentPosition() + posR);
  while( leftStepper.distanceToGo() || rightStepper.distanceToGo() )
  {
    leftStepper.run();
    rightStepper.run();
  }
}
*/
void stepLeft(int steps)
{
  for( int i = 0; i < abs( steps) ; i ++ )
  {
    if(steps > 0 )
      fsL();
    else
      bsL();
      
    delay(stepTime);
  }
}
void stepRight(int steps)
{
  for( int i = 0; i < abs( steps) ; i ++ )
  {
    if(steps > 0 )
      fsR();
    else
      bsR();
   
     delay(stepTime);
  }
}
 

void fsL()
 {
 //pd3 step pd4 dir
 cbi(PORTD,4);
 cbi(PORTD,3);
 sbi(PORTD,3);
 }
 void bsL()
 {
 sbi(PORTD,4);
 cbi(PORTD,3);
 sbi(PORTD,3);
 }
 void fsR()
 {
 //pd1 step, pd2 dir
 sbi(PORTD,2);
 cbi(PORTD,1);
 sbi(PORTD,1);
 }
 void bsR()
 {
 cbi(PORTD,2);
 cbi(PORTD,1);
 sbi(PORTD,1);
 }
#endif
