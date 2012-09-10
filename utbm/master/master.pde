/*
todo

*/
#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "fuelcell.h"

#define useSD
#define testMem
const int numPorts = 5;
const int slaveDelay = 500;
const int pollInterval = 5000; //ms
const int slaveSignal[numPorts] = { 6,4,2,A1,A3 };
SoftwareSerial serialPorts[numPorts] =
    { 
      //can't use pins 10->13 as they are used for SD
      //can't use A4 and A5 because they are used by wire library
      //rx,tx
    SoftwareSerial(7,6), //slave 0
    SoftwareSerial(5,4), //slave 1
    SoftwareSerial(3,2), //slave 2
    SoftwareSerial(A0,A1), //slave 4
    SoftwareSerial(A2,A3), //slave 3

    };

//pin defs
const int LED1 = 8;
const int LED2 = 9;
const int voltagePin = A0;
const int currentPin = A1;


//globals
boolean ledState = false;
MilliTimer getData,showData,statusLED;
int msgSize;
String fString;

void setup()  
{
  Serial.begin(9600);
  Serial.println(F("master"));

  Serial.println(F("init ports"));
  for( int port = 0; port < numPorts; port ++ )
  {
    serialPorts[port].begin(9600); //we'll use the software serial ports for reading, but not writing
    pinMode(slaveSignal[port],OUTPUT); //we'll use the TX pin instead for a simple signal
    digitalWrite(slaveSignal[port],LOW);
  }
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  msgSize = sizeof(Message);
  Serial.println(F("rtc"));
  setupRTC();
  Serial.println(F("ok"));
  Serial.println(F("sd"));
//  writeLogNumber(0); //init log number
  setupSD();
  Serial.println(F("ok"));
  fString = "started";
  writeError( fString );
  Serial.println(fString);
#ifdef testMem
Serial.print( F("mem: "));
Serial.println(freeMemory());
#endif
}

void loop()
{
  if( statusLED.poll(500) )
    flash(LED1);

  //wait till it's time to fetch data
  if( getData.poll(pollInterval))
  {
    #ifdef testMem
    Serial.print( "mem: ");
    Serial.println(freeMemory());
    #endif
    printDate();
    //int port = 0;
    for( int port = 0; port < numPorts; port ++ )
    {
      Serial.print( F("data req port:") );
      Serial.println( port );
      serialPorts[port].listen();
      //send the signal
      digitalWrite(slaveSignal[port],HIGH);
      //wait for slave to respond
      delay(slaveDelay);
      //reset signal
      digitalWrite(slaveSignal[port],LOW);
      //wait for data to arrive
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
            Serial.println(F("data ok" ));
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
  delay(50);
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

