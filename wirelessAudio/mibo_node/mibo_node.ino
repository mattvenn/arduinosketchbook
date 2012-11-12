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

SoftwareSerial xbeeSerial(XBEE_RX,XBEE_TX);
SFEMP3Shield MP3player;



void setup()
{
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  Serial.print( "started xbee node id:" );

  //EEPROM.write(idAddress,0); //set address
  ID = getId();

  Serial.println( ID );

  //start the shield
  MP3player.begin();

}
void loop()
{
  if( xbeeSerial.available() >= 2)
  {
    char command = xbeeSerial.read();
    Serial.print( "got: " );
    Serial.println( command );
    if( command == 'c' ) // a broadcast message from master to all clients
    {
      Serial.println( "got message from master: " );
      command = xbeeSerial.read();
      switch(command)
      {
      case 'i':
        {
          //we've got an id command, read it
          int id = serReadInt();
          //        Serial.print( "id: " );
          //       Serial.println( id );
          if( id == ID )
          {
            Serial.println( "our id called" );
            xbeeSerial.print( "mok" );
          }  
          else
          {
            //not for us so flush buffer
            xbeeSerial.flush();
          }          
          break;
        }
      case 's':
        Serial.println( "starting" );
        MP3player.playTrack(1);
        break;
      case 'e':
        Serial.println( "ending" );
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

byte getId()
{
  return EEPROM.read(idAddress);
}


