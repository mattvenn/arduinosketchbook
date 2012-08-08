#include <JeeLib.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
SoftwareSerial mySerial(2, 3);

const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;
const int AREF = 5000.0; //mv

const int idAddress = 0;
byte id;
MilliTimer send;
boolean ledState = false;

const float vDivider = 0.5;
const float iDivider = 0.5;

typedef struct
{
  float voltage;
  float current;
  byte status;
  byte id;
  int cksum;
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
  if(mySerial.available() >= size + 2)
  {
    if( mySerial.read() == '\001' && mySerial.read() == '\002' )
    {
      if( mySerial.available() >= size)
      {
        flash();
        ptr = (char *)&message;
        for(int count=0; count<size; count++)
          *(ptr+count) = mySerial.read();

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
}

void printData()
{
  Serial.print( "v:" ); Serial.println( message.voltage );
  Serial.print( "i:" ); Serial.println( message.current );
  Serial.print( "stat:" ); Serial.println( message.status, DEC );
  Serial.print( "id:" ); Serial.println( message.id, DEC );
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
