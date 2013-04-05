#ifdef testSteppers


#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#define BIT_TST(REG, bit, val) ( ( (REG & (1UL << (bit) ) ) == ( (val) << (bit) ) ) )


//const int steps = 200;
int stepTime = 2;
const int SUPERFAST_ACCELERATION = 6000;
//AccelStepper leftStepper(fsL,bsL);
//AccelStepper rightStepper(fsR,bsR);

//#include <digitalWriteFast.h>

void initSteppers()
{
  setPWM(config.default_pwm);
  setSpeed(config.maxSpeed);
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

void powerSave(boolean save)
{
  if(save==true)
  {
    lowpower = true;
    setPWM(config.lowpower_pwm);
    Serial.println("low power mode");
  }
  else
  {
    lowpower = false;
    setPWM(config.default_pwm);
    Serial.println("default power mode");
  }
}

void calibrate(int steps)
{

  stepLeft(steps);
  Serial.print("moved ");Serial.print(steps);Serial.println("steps. How many mm?");
  while(Serial.available()==0)
  {
    delay(100);
  }
  float mm = serReadFloat();
  Serial.println(steps/mm);

}
  
void home()
{
  //got to do this or our pwm settings are lost when we move the motors
  powerSave(false);
  penUp();

  setMS(LOW,LOW);
  
  //setup pwm so can pull the right hand side
  //set pwmL to high
  setPWML(config.HOME_PWM_HIGH);
  //set pwmR to low
  setPWMR(config.HOME_PWM_LOW);
  
  //get to left limit
  setSpeed( config.HOME_SPEED );
  findLeftLimit();
  
  //default pwm
  setPWM(config.default_pwm);
  //release any tesnsion
  int tension_release_steps = 100;
  stepLeft(tension_release_steps);
  int steps=findRightLimit();
  
  a1 = (142 + config.hanger_l)*config.stepsPerMM + steps + tension_release_steps;
  b1 = (152 + config.hanger_l)*config.stepsPerMM;
  FK(a1,b1); //this updates x and y
  
  Serial.println(a1/config.stepsPerMM - config.hanger_l);
  Serial.println(b1/config.stepsPerMM - config.hanger_l);
  Serial.println(x1/config.stepsPerMM);
  Serial.println(y1/config.stepsPerMM);
  
  setPWM(config.default_pwm);
  
  //move to center point
  drawLine(config.w/2,config.w/2);

  setSpeed(config.maxSpeed);
  calibrated=true;

}

int findRightLimit()
{
  int steps = 0;

  //while limit l is high, wind l motor
  while( digitalRead( LIMITR ) == LOW )
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

  

  //while limit l is high, wind l motor
  while( digitalRead( LIMITL ) == LOW )
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
  Serial.print( "set ms1: ");
  Serial.println(ms1);
  Serial.print( "set ms2: ");
  Serial.println(ms2);
}

void setSpeed(int speed)
{
  //leftStepper.setMaxSpeed(speed);
  //rightStepper.setMaxSpeed(speed);
  stepTime = speed;
  Serial.print("set speed:");
  Serial.println(speed);
}
void setAccel(int accel)
{
  //leftStepper.setAcceleration(accel);
  //rightStepper.setAcceleration(accel);
  Serial.print( "set accel: " );
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
  if(lowpower)
    powerSave(false);
  for( int i = 0; i < abs( steps) ; i ++ )
  {
    if(steps > 0 )
      fsL();
    else
      bsL();
      
    delayMicroseconds(stepTime);
  }
}
void stepRight(int steps)
{
  if(lowpower)
    powerSave(false);
  for( int i = 0; i < abs( steps) ; i ++ )
  {
    if(steps > 0 )
      fsR();
    else
      bsR();
   
     delayMicroseconds(stepTime);
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
