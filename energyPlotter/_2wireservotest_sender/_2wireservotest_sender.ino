#define mosfetPin 3
#define ledPin 13
#include <Servo.h>
Servo myservo;
int delayT = 1;
void setup()
{
  Serial.begin(9600);
  myservo.attach(9);
  Serial.println( "starting" );
  pinMode(mosfetPin, OUTPUT );
  pinMode(ledPin, OUTPUT );
  setPowerPin( LOW );
  Serial.println( "waiting for reset" );
  delay(2000);
  Serial.println( "starting" );

}
int pos = 0;
void loop()
{
  /*
    for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  delay( 4000 );
 // myservo.write(delayT );
  Serial.println( delayT );
  //lift servo
  pulsePower( 1, 20 );
  delay(4000);
    pulsePower( 1, 600 );
    
    
  
}


void pulsePower( int pulseLength, int delayTime )
{
      setPowerPin( HIGH   );
      delay( pulseLength );
      setPowerPin( LOW );
      delay( delayTime );
            setPowerPin( HIGH   );
            delay( pulseLength );
      setPowerPin( LOW );

  
}
void setPowerPin( boolean state )
{
    digitalWrite(mosfetPin, ! state );
  digitalWrite(ledPin, ! state );
}

