
void pulsePower( int pulseLength, int delayTime )
{
  //Serial.print( "pulsing servo line: " );
  //Serial.println( delayTime );

  //turn on power to the board
  setPowerPin(HIGH);

  //wait the specified length
  delay(delayTime);

  //send the signal pulse
  setPowerPin( LOW   );
  delay( pulseLength );
  setPowerPin( HIGH );

  //wait for servo to move, think this is about right
  delay(2*delayTime);

  //then turn off the power
  setPowerPin(LOW);
  //wait for a bit longer to allow board to reset (multiple calls can stop the servo working properly)
  delay(500);
}
void setPowerPin( boolean state )
{
  digitalWrite(SERVO, !state );
//  digitalWrite(led, ! state );
}


