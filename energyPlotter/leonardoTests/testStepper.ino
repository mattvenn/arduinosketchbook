#ifdef testSteppers

  const int maxSpeed = 200;
  const int pwmVal = 255;
  const int steps = 200;
Stepper leftStepper = Stepper( steps, DIRL, STEPL );
Stepper rightStepper = Stepper( steps, DIRR, STEPR );


void initSteppers()
{
   analogWrite(PWMR, pwmVal );
 analogWrite(PWML, pwmVal );
  leftStepper.setSpeed(maxSpeed);
  rightStepper.setSpeed(maxSpeed);

/* micro stepping stuff
 LOW, LOW = no micro stepping : works
 HIGH, LOW = half step : works
 LOW, HIGH = quarter : works on left side
 high, high = eigth step :works on left side
 */
   digitalWrite(MS1, HIGH ); //both low is no microstep 
  digitalWrite(MS2, HIGH);

 
}

void moveSteppers()
{
  Serial.println( "step");
 
    leftStepper.step(800);
    leftStepper.step(-800);
    rightStepper.step(800);
    rightStepper.step(-800);
}

/*
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
*/
#endif



