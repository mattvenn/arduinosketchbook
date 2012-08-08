#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial mySerial(2, 3);

//pin defs
const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;

//globals
boolean ledState = false;

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
  mySerial.begin(9600);
  pinMode(LED,OUTPUT);
}

void loop()
{
  int size = sizeof(Message);
  char * ptr;
  //wait for data of the right size
  if(mySerial.available() >= size + 2)
  {
    //has to have correct header
    if( mySerial.read() == '\001' && mySerial.read() == '\002' )
    {
      flash();
      ptr = (char *)&message;
      for(int count=0; count<size; count++)
        *(ptr+count) = mySerial.read();

      //check the data is OK
      if( validateCheckSum() )
      {
        Serial.println("got data:" );
        printData();
      }
      else
      {
        Serial.println("data corrupt" );
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
}

void flash()
{
  digitalWrite(LED,ledState);
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
  for (XOR = 0, i = 0; i < sizeof(Message); i++) {
    XOR ^= *ptr ++;
  }
  return XOR;
}
