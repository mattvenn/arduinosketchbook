#define XBEE_RX 2
#define XBEE_TX 3
#include <LiquidCrystal.h>

#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);

const int waitForNodePoll = 200;
const int numNodes = 10;
const int playButton = 8;
const int stopButton = 10;
const int volumeKnob = A0;
double lastKnobTime, lastPollTime;

int volume, lastVolume;

SoftwareSerial xbeeSerial(XBEE_RX,XBEE_TX);

void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.println( "started xbee master" );
  lcd.begin(16, 2);
  displayStartScreen();
  
  pinMode( stopButton, INPUT );
  pinMode( playButton, INPUT );
  
  pollNodes();
  stopNodes();
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
  if( digitalRead( playButton ) )
  {
    startNodes();
  }
  else if( digitalRead( stopButton ) )
  {
    stopNodes();
  }

  volume = map( analogRead( volumeKnob ), 0 , 1024, 100, 0 );
  
  lcd.setCursor(8,1);
  lcd.print( "vol:    ");
  lcd.setCursor(13,1);
  lcd.print( volume );
  
  if( millis() - lastKnobTime > 500 )
  {
    lastKnobTime = millis();
    if( lastVolume != volume ) // knob has changed
    {
      sendVolume();
    }
    lastVolume = volume;

  }
  
  if( millis() - lastPollTime > 1000 )
  {
    lastPollTime = millis();
    pollNodes();
  }    
 }

void sendVolume()
{
  xbeeSerial.print( "cv" );
  xbeeSerial.print( volume );
  Serial.print( "sending vol:" );
  Serial.println( volume );
  
  delay(100);
}
void startNodes()
{
  xbeeSerial.print( "cs" );
  Serial.println( "starting" );
  lcd.setCursor(0,1);
  lcd.print( "playing");
  delay(100);
}
void stopNodes()
{
  xbeeSerial.print( "ce" );
  Serial.println( "ending" );
  lcd.setCursor(0,1);
  lcd.print( "stopped");
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
    
    delay(waitForNodePoll); //transmission delay
    
    lcd.setCursor(i,0);
    if( xbeeSerial.available() )
    {
      Serial.println( " OK" );
      //don't care what the answer is
      xbeeSerial.flush();
      lcd.print( '*' );
      
    }
    else
    {
      Serial.println( " not online" );
      lcd.print( '.' );
    }
  }
}

void displayStartScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "mibo master");
  delay(1000);
  lcd.clear();
}
