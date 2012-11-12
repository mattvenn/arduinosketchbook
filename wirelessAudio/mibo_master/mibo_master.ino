#define XBEE_RX 2
#define XBEE_TX 3

#include <SoftwareSerial.h>

const int numNodes = 2;

SoftwareSerial xbeeSerial(XBEE_RX,XBEE_TX);
void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.println( "started xbee master" );
}
void loop()
{
  if( Serial.available() )
  {
    delay(50);
    char command = Serial.read();
    switch( command )
    {
      case 'i':
        pollNodes();
        break;
      case 's':
        startNodes();
        break;
      case 'e' :
        stopNodes();
        break;
      default:
        Serial.println( "bad command:" );
        Serial.println( command );
    }
  }
 }

void startNodes()
{
  xbeeSerial.print( "cs" );
  Serial.println( "starting" );
  delay(100);
}
void stopNodes()
{
  xbeeSerial.print( "ce" );
  Serial.println( "ending" );
  delay(100);
}
void pollNodes()
{
  Serial.println( "checking for online nodes..." );
  for( int i = 0; i < numNodes ; i ++ )
  {
    Serial.print( "node: " );
    Serial.print( i );
    xbeeSerial.print( "ci" );
    xbeeSerial.print( i );
    
    delay(100); //transmission delay
    
    if( xbeeSerial.available() )
    {
      Serial.println( " OK" );
      //don't care what the answer is
      xbeeSerial.flush();
      
    }
    else
    {
      Serial.println( " not online" );
    }
  }
}
