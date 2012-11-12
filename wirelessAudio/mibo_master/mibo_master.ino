#define XBEE_RX 2
#define XBEE_TX 3
#include <LiquidCrystal.h>

#include <SoftwareSerial.h>
LiquidCrystal lcd(12, 11, 5, 4, 7, 6);

const int switchDebounceTime = 200;

const int numNodes = 10;
const int playButton = 8;
const int stopButton = 10;
const int volumeKnob = A0;
double lastKnobTime, lastPollTime;
double nodeStatus [numNodes];
int volume, lastVolume;
boolean isplaying = false;

SoftwareSerial xbeeSerial(XBEE_RX,XBEE_TX);

 int onlineChar = 0;
 int offlineChar = 1;

byte offline[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};

byte online[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};


void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.println( "started xbee master" );

  lcd.begin(16, 2);
  lcd.createChar(onlineChar, online);  
  lcd.createChar(offlineChar, offline);  
  
  displayStartScreen();

  pinMode( stopButton, INPUT );
  pinMode( playButton, INPUT );

  stopplay();
  pollNodes();
}
void loop()
{
  if( digitalRead( playButton ) )
  {
    play();
    
  }
  else if( digitalRead( stopButton ) )
  {
    stopplay();
  }


  //volume knob stuff, only check every 200ms
  if( millis() - lastKnobTime > 200 )
  {
    volume = map( analogRead( volumeKnob ), 0 , 1024, 100, 0 );
    lastKnobTime = millis();
    if( lastVolume != volume ) // knob has changed
    {
      //only send if knob changed
      sendVolume();
    }
    lastVolume = volume;
  }

  //send message to all nodes
  if( millis() - lastPollTime > 1000 )
  {
    lastPollTime = millis();
    pollNodes();
  }    
}

void stopplay()
{

    isplaying = false;
    Serial.println( "stopping" );
    lcd.setCursor(0,1);
    lcd.print( " stopped");
    delay(switchDebounceTime);
}
void play()
{
    isplaying = true;
    Serial.println( "starting" );
    lcd.setCursor(0,1);
    lcd.print( " playing");
    delay(switchDebounceTime);
}
void sendVolume()
{
  xbeeSerial.print( "cv" );
  xbeeSerial.write( volume );
  Serial.print( "sending vol:" );
  Serial.println( volume );
  lcd.setCursor(8,1);
  lcd.print( "        ");
  lcd.setCursor(12,1);
  lcd.print( volume );
}


void pollNodes()
{
  //deal with packet loss by continualling telling nodes what to do!
  if( isplaying )
    xbeeSerial.print( "cs" );
  else
    xbeeSerial.print( "ce" );

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
  lcd.setCursor(3,0);
  for( int node = 0; node < numNodes ; node ++ )
  {
    if( millis() - nodeStatus[node] < 5000 )
      lcd.write( onlineChar );
    else
      lcd.write( offlineChar );
  }
}

void displayStartScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "  mibo  master");
  lcd.setCursor(0,1);
  lcd.print( "    starting");
  delay(1000);
  lcd.clear();
}

