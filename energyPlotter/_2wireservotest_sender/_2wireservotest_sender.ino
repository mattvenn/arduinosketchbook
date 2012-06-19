#define mosfetPin 3
#define ledPin 13
void setup()
{
  pinMode(mosfetPin, OUTPUT );
  pinMode(ledPin, OUTPUT );
  setPowerPin( LOW );

  delay( 500 );
  //lift servo
  pulsePower( 1, 50 );
  delay( 2000 );
  //lower servo
  pulsePower( 2, 50 );
  //end
  
}
void pulsePower( int times, int delayTime )
{
  for( int i = 0; i < times ; i ++ )
  {
      setPowerPin( HIGH   );
      delay( delayTime );
      setPowerPin( LOW );
      delay( delayTime );
  }
}
void setPowerPin( boolean state )
{
    digitalWrite(mosfetPin, state );
  digitalWrite(ledPin, ! state );
}

void loop()
{
}
  
