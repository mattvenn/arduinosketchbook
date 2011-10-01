NewSoftSerial energyRobotSerial(ROBOT_RX,ROBOT_TX);

void energyRobotSetup()  
{
  energyRobotSerial.begin(9600);

}

void sendRobotData()
{
  if( minutes >= 0 )
  {
    //Serial.println( "sending to energy robot" );
    
    int intPower = (int)power;
    Serial.print( "e" );
     Serial.print( intPower );
  Serial.print( "," );
  Serial.println( minutes );
  //nss doesn't work
  energyRobotSerial.print( "e" );
  energyRobotSerial.print( intPower );
  energyRobotSerial.print( "," );
  energyRobotSerial.println( minutes );
  }
}
