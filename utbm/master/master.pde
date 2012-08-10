#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>


const int numPorts = 2;
const int pollInterval = 2000; //ms
SoftwareSerial serialPorts[numPorts] =
    { 
        SoftwareSerial(2,3),
        SoftwareSerial(4,5)
    };

//pin defs
const int LED1 = 8;
const int LED2 = 9;
const int voltagePin = A0;
const int currentPin = A1;

#define DATAFILE "data3.txt"
#define ERRORFILE "error.txt"

//globals
boolean ledState = false;
MilliTimer getData,showData,statusLED;
int msgSize;
String fString;

//data struct
typedef struct
{
  float voltage;
  float current;
  byte status;
  byte id;
  int cksum;
  unsigned long uptime;
} Message;
Message message;

void setup()  
{
  Serial.begin(57600);
  Serial.println("master"); 
  Serial.println("init ports");
  for( int i = 0; i < numPorts; i ++ )
  {
    serialPorts[i].begin(9600);
  }
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  msgSize = sizeof(Message);
  Serial.println("rtc setup");
  setupRTC();
  Serial.println("sd setup");
  setupSD();
  fString = "started OK";
  writeError( fString );
}

void loop()
{
  if( statusLED.poll(500) )
    flash(LED1);
  if( Serial.available() )
  {
    char c = Serial.read();
    switch(c)
    {
        case 'd':
          readFile(DATAFILE);
        break;
        case 'e':
          readFile(ERRORFILE);
        break;
        default:
          Serial.println( "bad command" );
        break;
    }
  }
  //wait till it's time to fetch data
  if( getData.poll(pollInterval))
  {
    printDate();
    for( int port = 0; port < numPorts; port ++ )
    {
      Serial.print( "requesting data from slave on port:" );
      Serial.println( port );
      serialPorts[port].listen();
      serialPorts[port].write('\001');
      //wait for slave to respond
      delay(100);
      if(serialPorts[port].available() >= msgSize + 2)
      {
        //has to have correct header
        if( serialPorts[port].read() == '\001' && serialPorts[port].read() == '\002' )
        {
          flash(LED2);
          char * ptr = (char*)&message;
          for(int count=0; count<msgSize; count++)
            *(ptr+count) = serialPorts[port].read();

          //check the data is OK
          if( validateCheckSum() )
          {
            Serial.println("got data OK" );
            //printData();
            writeData();
          }
          else
          {
            fString = "corrupt data from port "; 
            fString += port;
            writeError( fString );
            Serial.println("data corrupt" );
          }
        }
      }
      else
      {
        Serial.println( "slave didn't respond");
        fString = "slave didn't respond on port ";
        fString += port;
        writeError( fString );
      }
    }
  }
}

//prints formatted data
void printData()
{
  Serial.print( "v:" ); Serial.println( message.voltage );
  Serial.print( "i:" ); Serial.println( message.current );
  Serial.print( "stat:" ); Serial.println( message.status, DEC );
  Serial.print( "id:" ); Serial.println( message.id, DEC );
  Serial.print( "uptime:" ); Serial.println( message.uptime );
  Serial.println("-------");
}

void flash(int LED)
{
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
}

boolean validateCheckSum()
{
  int sentCksum = message.cksum;
  message.cksum = 0;
  if( getCheckSum() != sentCksum )
    return false;
  Serial.print( "valid cksum: " ); Serial.println( sentCksum, DEC );
  return true;
}

//XOR checksum
int getCheckSum() 
{
  int i;
  int XOR;
  char * ptr = (char*)&message;
  for (XOR = 0, i = 0; i < msgSize; i++) {
    XOR ^= *ptr ++;
  }
  return XOR;
}
