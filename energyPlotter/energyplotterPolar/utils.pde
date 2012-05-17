boolean calibrated = false;
#define TENSL digitalRead(TENS_L)
#define TENSR digitalRead(TENS_R)
void calibrate()
{
  Serial.println("calibration" );
  int ending = 0;

  //get both tight
  while( ! TENSL && ! TENSR )
  {
    if( ! TENSL )
      step( LEFT, -1 );
    if( ! TENSR )
      step( RIGHT, -1 );
  }

  while( ! calibrated )
  {
    int countl = 0, countr = 0;
    while( TENSL )
    {
      step( RIGHT, 1 );
      countr ++;
      if( countr > 10 && ending > 10 )
      {

        calibrated = true;
        Serial.println( "finished left side" );
        break;
      }

    }
    //wind in left till tens
    while( ! TENSL )
    {
      step( LEFT, -1 );
      countl ++;
    }

    //detect when we're in the jiggling phase!
    if( countl < 10 && countr < 10 )
    {
      ending ++;
      Serial.print( "ending=" );
      Serial.println( ending );
    }

  }
  
  int stringLength = 0;
  calibrated = false;
  ending = 0;

  //get right tight
  while( ! TENSR )
  {
    step( RIGHT, -1 );
  }

  while( ! calibrated )
  {
    int countl = 0, countr = 0;
    while( TENSR )
    {
      step( LEFT, 1 );
      countl ++;
      if( countl > 10 && ending > 10 )
      {

        calibrated = true;
        Serial.println( "finished right side" );
        break;
      }

    }
    //wind in left till tens
    while( ! TENSR )
    {
      step( RIGHT, -1 );
      stringLength ++;
      countr ++;
    }

    //detect when we're in the jiggling phase!
    if( countl < 10 && countr < 10 )
    {
      ending ++;
      Serial.print( "ending=" );
      Serial.println( ending );
    }

  }
  Serial.println( "string length");
  Serial.println( stringLength);

   //w= stringLength; can't do this with a const
   //h = w;
   //ceiling = h / 4; //5; // 10*StepUnit;
   //margin = w / 4; //4;

  // Coordinates of current (starting) point
  x1= w;
  y1= 0;

  // Approximate length of strings from marker to staple
  a1= 0;
  b1= stringLength;

//  moveTo( w/2, h/2 );


}
void step( int stepper, int steps )
{
  if( steppersOn != true )
    turnOnSteppers();
  stepping = true; 
  if( stepper == LEFT )
  {

    leftStepper.step( leftStepDir * steps );

  }
  else if( stepper == RIGHT )
  {
    rightStepper.step( rightStepDir * steps );
  }
  stepping = false;
}         

void turnOffSteppers()
{
  //return;
  if( steppersOn != true )
    return;
  Serial.println( "turning pwm low" );
  for( int i = PWM_HIGH ; i >= PWM_LOW; i -- )
  {
    analogWrite( STEP_PWM, i );
    delay(PWM_CHANGE_DELAY);
  }
  //  Serial.println( "loopdone" );
  steppersOn = false;
}

void turnOnSteppers()
{
  //  return;
  if( steppersOn == true )
    return;
  Serial.println( "turning pwm high" );
  for( int i = PWM_LOW ; i <= PWM_HIGH; i ++ )
  {
    analogWrite( STEP_PWM, i );
    delay(PWM_CHANGE_DELAY);
  }
  //  Serial.println( "loopdone" );
  steppersOn = true;
  //Serial.println( "done");
}

int serReadInt()
{
  int i, serAva;                           // i is a counter, serAva hold number of serial available
  char inputBytes [7];                 // Array hold input bytes
  char * inputBytesPtr = &inputBytes[0];  // Pointer to the first element of the array

  if (Serial.available()>0)            // Check to see if there are any serial input
  {
    delay(5);                              // Delay for terminal to finish transmitted
    // 5mS work great for 9600 baud (increase this number for slower baud)
    serAva = Serial.available();  // Read number of input bytes
    for (i=0; i<serAva; i++)   
    {
      char readChar = (char)Serial.read();   // Load input bytes into array
      if( readChar == ',' )
        break;
      inputBytes[i] = readChar;
    }
    inputBytes[i] =  '\0';             // Put NULL character at the end
    return atoi(inputBytesPtr);    // Call atoi function and return result
  }
  else
    return -1;                           // Return -1 if there is no input
}

int xbeeserReadInt()
{
  int i, serAva;                           // i is a counter, serAva hold number of serial available
  char inputBytes [7];                 // Array hold input bytes
  char * inputBytesPtr = &inputBytes[0];  // Pointer to the first element of the array

  if (xbeeSerial.available()>0)            // Check to see if there are any serial input
  {
    delay(5);                              // Delay for terminal to finish transmitted
    // 5mS work great for 9600 baud (increase this number for slower baud)
    serAva = xbeeSerial.available();  // Read number of input bytes
    for (i=0; i<serAva; i++)   
    {
      char readChar = (char)xbeeSerial.read();   // Load input bytes into array
      if( readChar == ',' )
        break;
      inputBytes[i] = readChar;
    }
    inputBytes[i] =  '\0';             // Put NULL character at the end
    return atoi(inputBytesPtr);    // Call atoi function and return result
  }
  else
    return -1;                           // Return -1 if there is no input
}
/**
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 64: 
      mode = 0x03; 
      break;
    case 256: 
      mode = 0x04; 
      break;
    case 1024: 
      mode = 0x05; 
      break;
    default: 
      return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } 
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } 
  else if(pin == 3 || pin == 11) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 32: 
      mode = 0x03; 
      break;
    case 64: 
      mode = 0x04; 
      break;
    case 128: 
      mode = 0x05; 
      break;
    case 256: 
      mode = 0x06; 
      break;
    case 1024: 
      mode = 0x7; 
      break;
    default: 
      return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

