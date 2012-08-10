#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>

const int numPorts = 2;
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

//globals
boolean ledState = false;
MilliTimer getData,showData;
int msgSize;

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
  Serial.begin(9600);
  Serial.println("master"); 
  Serial.println("init ports");
  for( int i = 0; i < numPorts; i ++ )
  {
    serialPorts[i].begin(9600);
  }
  pinMode(LED1,OUTPUT);
  msgSize = sizeof(Message);
  Serial.println("rtc setup");
  setupRTC();
  Serial.println("sd setup");
  setupSD();
}

void loop()
{
  if( showData.poll(5000) )
    readData();

  //wait till it's time to fetch data
  if( getData.poll(1000))
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
          flash();
          char * ptr = (char*)&message;
          for(int count=0; count<msgSize; count++)
            *(ptr+count) = serialPorts[port].read();

          //check the data is OK
          if( validateCheckSum() )
          {
            Serial.println("got data:" );
            printData();
            writeData();
          }
          else
          {
            Serial.println("data corrupt" );
          }
        }
      }
      else
      {
        Serial.println( "slave didn't respond");
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

void flash()
{
  digitalWrite(LED1,ledState);
  ledState = !ledState;
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
