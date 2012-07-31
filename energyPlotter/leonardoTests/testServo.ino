
void pulsePower( int pulseLength, int delayTime )
{
    Serial.print( "pulsing servo line: " );
    Serial.println( delayTime );
    setPowerPin(LOW);
    delay(2000);
      setPowerPin( HIGH   );
      delay( pulseLength );
      setPowerPin( LOW );
      delay( delayTime );
            setPowerPin( HIGH   );
            delay( pulseLength );
      setPowerPin( LOW );
      //wait for servo to move
      delay(2000);
    setPowerPin(HIGH);
  
}
void setPowerPin( boolean state )
{
    digitalWrite(SERVO,  state );
  digitalWrite(led, ! state );
}

