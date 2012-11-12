#define XBEE_RX 2
#define XBEE_TX 3
#include <LiquidCrystal.h>

#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);

const int switchDebounceTime = 500;

const int numNodes = 10;
const int playButton = 8;
const int stopButton = 10;
const int volumeKnob = A0;
double lastKnobTime, lastPollTime;
double nodeStatus [numNodes];
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

  
  
  if( millis() - lastKnobTime > 200 )
  {
    volume = map( analogRead( volumeKnob ), 0 , 1024, 100, 0 );
  
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
  xbeeSerial.write( volume );
  Serial.print( "sending vol:" );
  Serial.println( volume );
  lcd.setCursor(8,1);
  lcd.print( "vol:    ");
  lcd.setCursor(13,1);
  lcd.print( volume );
  
  delay(200);
}
void startNodes()
{
  xbeeSerial.print( "cs" );
  delay(200);
  xbeeSerial.print( "cs" );
  Serial.println( "starting" );
  lcd.setCursor(0,1);
  lcd.print( "playing");
  delay(switchDebounceTime);
}
void stopNodes()
{
  xbeeSerial.print( "ce" );
  delay(200);
  xbeeSerial.print( "ce" );
  Serial.println( "ending" );
  lcd.setCursor(0,1);
  lcd.print( "stopped");
  delay(switchDebounceTime);
}


void pollNodes()
{
  Serial.println( "checking for online nodes..." );
  xbeeSerial.print( "ci" );
  
  while( xbeeSerial.available() )
  {
    if( xbeeSerial.read() == 'm' )
    {
      int node = xbeeSerial.read();
      Serial.print( "got poll from: " );
      Serial.println( node );
      nodeStatus[node] = millis();
    }
  }
  lcd.setCursor(0,0);
  for( int node = 0; node < numNodes ; node ++ )
  {
    if( millis() - nodeStatus[node] > 5000 )
      lcd.print(".");
    else
      lcd.print("*");
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
