#define sigPin 4
#define ledPin 13
const int expPulse = 50;
const int pMargin = 10;

long int pulseTime;
long int lastPulse;
boolean pulse, doublePulse, pulseStart = false;

#include <Servo.h> 
Servo myservo;  
#define servoPin 9
const int servoUpPos = 0;
const int servoDownPos = 180;

void setup()
{
  pinMode( sigPin, INPUT );
  pinMode( ledPin, OUTPUT );
  digitalWrite( ledPin, LOW );
  digitalWrite( sigPin, LOW );
  myservo.attach(servoPin);
  Serial.begin(9600);
}


void loop()
{
  
  if( digitalRead( sigPin ) == LOW )
  {
    pulseTime = millis();
    while( digitalRead( sigPin ) == LOW )
    {
      delay(1);
    }
    pulseTime = millis() - pulseTime;
   // Serial.println( pulseTime );
    if( pulseTime > expPulse - pMargin && pulseTime < expPulse + pMargin )
    {
//      if( pulse == True && lastPulse - millis() < 100 )
     // Serial.println( "got pulse" );
     // Serial.println( millis() - lastPulse );    
      
      //check for a double pulse, did we have a single pulse 2 * pulseLengths ago?
      if( millis() - lastPulse < 2 * ( expPulse + pMargin ) )
      {
        Serial.println( "double pulse" );
        doublePulse = true;
        pulseStart = false;
      }
      else
      {
      //  Serial.println( "could be a start of a single pulse" ); // or could be start of a double
        pulseStart = true;
      }
      
      lastPulse = millis();
    }
  
  }
  
    if( pulseStart )
    {
   //   Serial.println( millis() - lastPulse );
      if( millis() - lastPulse > 2 * ( expPulse + pMargin ) )
      {
         pulse = true;
         Serial.println( "single pulse" );
         pulseStart = false;
      }
    }
    
    if( pulse )
    {
      flashLed( 1 );
        myservo.write( servoUpPos );
       pulse = false;
    }
    if( doublePulse )
    {
        flashLed( 2 );
        myservo.write( servoDownPos );
        doublePulse = false;
    }
}

void flashLed( int times )
{
  for( int i = 0; i < times ; i ++ )
  {
      digitalWrite( ledPin, HIGH   );
      delay( 150 );
       digitalWrite( ledPin, LOW   );
      delay( 150 );


  }
}

