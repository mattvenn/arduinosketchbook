
void pulsePower( int pulseLength, int delayTime )
{
  Serial.print( "pulsing servo line: " );
  Serial.println( delayTime );

  //turn on power to the board
  setPowerPin(HIGH);

  //wait the specified length
  delay(delayTime);

  //send the signal pulse
  setPowerPin( LOW   );
  delay( pulseLength );
  setPowerPin( HIGH );

  //wait for servo to move
  delay(1000);

  //then turn off the power
  setPowerPin(LOW);


}
void setPowerPin( boolean state )
{
  digitalWrite(SERVO, !state );
//  digitalWrite(led, ! state );
}


