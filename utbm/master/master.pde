#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>

#include "fuelcell.h"

//#define testMem 1

const int numPorts = 5;
const int slaveDelay = 150;
const int pollInterval = 4000; //ms
SoftwareSerial serialPorts[numPorts] =
    { 
      //can't use pins 10->13 as they are used for SD
      //can't use A4 and A5 because they are used by wire library
    SoftwareSerial(7,6), //slave 0
    SoftwareSerial(5,4), //slave 1
    SoftwareSerial(3,2), //slave 2
    SoftwareSerial(A3,A2), //slave 3
    SoftwareSerial(A1,A0), //slave 4

    };

//pin defs
const int LED1 = 8;
const int LED2 = 9;
const int voltagePin = A0;
const int currentPin = A1;

#define DATAFILE "data5.txt"
#define ERRORFILE "error5.txt"

//globals
boolean ledState = false;
MilliTimer getData,showData,statusLED;
int msgSize;
String fString;

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
  pinMode(LED2,OUTPUT);
  msgSize = sizeof(Message);
  Serial.println("rtc");
  setupRTC();
  Serial.println("ok");
  Serial.println("sd");
  setupSD();
  Serial.println("ok");
  fString = "started";
  writeError( fString );
  Serial.println("started");
#ifdef testMem
Serial.print( "mem: ");
Serial.println(freeMemory());
#endif
}

void loop()
{
  if( statusLED.poll(500) )
    flash(LED1);
    /*
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
  */
  //wait till it's time to fetch data
  if( getData.poll(pollInterval))
  {
    #ifdef testMem
    Serial.print( "mem: ");
    Serial.println(freeMemory());
    #endif
    printDate();
    for( int port = 0; port < numPorts; port ++ )
    {
      Serial.print( "data req:" );
      Serial.println( port );
      serialPorts[port].listen();
      serialPorts[port].write('\001');
      //wait for slave to respond
      delay(slaveDelay);
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
            Serial.println("data ok" );
            //printFuelCellStatus();
            writeData();
          }
          else
          {
            fString = "corrupt data from "; 
            fString += port;
            Serial.println(fString);
            writeError( fString );
          }
        }
        else
        {
            fString = "bad header from "; 
            fString += port;
            Serial.println(fString);
            writeError( fString );
        }
      }
      else
      {
        fString = "no reply from ";
        fString += port;
        Serial.println(fString);
        writeError( fString );
      }
    }
  }
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
 /*
 Serial.print( "valid cksum: " );
  Serial.println( sentCksum, DEC );
  */
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

#ifdef testMem
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
int free_memory;
  
    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
};
#endif

