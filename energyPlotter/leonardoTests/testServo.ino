void initServo()
{
  setPowerPin(HIGH);
}
void penUp()
{
  pulsePower(2);
delay(100);
  pulsePower(2);
  delay(1000); //these could be made shorter if the gondola code is better
}

void penDown()
{
  pulsePower(2);
  delay(50);
  pulsePower(2);
    delay(1000); //these could be made shorter
}

void pulsePower(  int delayTime )
{
  setPowerPin(LOW);
  //wait the specified length
  delay(delayTime);
  setPowerPin(HIGH);

}
void setPowerPin( boolean state )
{
  digitalWrite(SERVO, state );
//  digitalWrite(led, ! state );
}


