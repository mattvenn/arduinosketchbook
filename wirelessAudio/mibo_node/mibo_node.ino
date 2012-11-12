#include <SoftwareSerial.h>

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>
#include <EEPROM.h>

#define DEBUG 0

#define XBEE_RX A0
#define XBEE_TX A1

const int idAddress = 0;
byte ID;
double lastPollRX;
double lastPollTX;
SoftwareSerial xbeeSerial(XBEE_RX,XBEE_TX);
SFEMP3Shield MP3player;
boolean isplaying = false;
const int radioStatusLED = A2;


void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.print( "started xbee node id:" );

  /*set address: uncomment line below and change the 0 to the node's id (0-9).
  Then compile and upload. 
  Then recomment out line, compile and upload
  */
  //EEPROM.write(idAddress,0); 
  ID = getId();

  pinMode( radioStatusLED, OUTPUT );
  digitalWrite( radioStatusLED, LOW );
  Serial.println( ID );

  //start the shield
  MP3player.begin();

}
void loop()
{
  if( isplaying && ! MP3player.isPlaying() )
  {
    //we need to restart it
    MP3player.playTrack(1);
  }


  //update radio status LED
  if( millis() - lastPollRX > 5000 )
  {
    //comms problem
    digitalWrite( radioStatusLED, false );
  }
  else
  {
    digitalWrite( radioStatusLED, true );
  }
  //send a poll to master
  if( millis() - lastPollTX > 1000 )
  {
    lastPollTX = millis();
    Serial.println( "send poll" );
    xbeeSerial.print( "m" );
    xbeeSerial.write( ID );
  }
  if( xbeeSerial.available() >= 2)
  {
    char command = xbeeSerial.read();
   // Serial.print( "got: " );
    //Serial.println( command );
    if( command == 'c' ) // a broadcast message from master to all clients
    {
      Serial.println( "got message from master" );
      lastPollRX = millis();

      command = xbeeSerial.read();
      switch(command)
      {
      case 'v':
        {
          int volume = xbeeSerial.read();
          if( volume > 0 )
          {
            Serial.print( "got vol: " );
            Serial.println( volume );
            setVolume(volume);
          }
          break;
        }
      case 's':
        Serial.println( "starting" );
        isplaying = true;
        MP3player.playTrack(1);
        break;
      case 'e':
        Serial.println( "ending" );
        isplaying = false;
        MP3player.stopTrack();
        break;
      default:
        Serial.println( "bad command" );
        Serial.flush();
      }
    }
    else
    {
      //a message to the server, so ignore
      Serial.flush();
    }

  }    

}

//Set VS10xx Volume Register
void setVolume(unsigned char volume)
{
  Mp3WriteRegister(SCI_VOL, 100 - volume, 100 - volume);
}

byte getId()
{
  return EEPROM.read(idAddress);
}



