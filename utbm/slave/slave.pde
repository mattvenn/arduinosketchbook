#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>

SoftwareSerial mySerial(2, 3);

//pin defs
const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;

//elect defs
const int AREF = 5000.0; //mv
const float vDivider = 0.5;
const float iDivider = 0.5;

//globals
const int idAddress = 0;
byte id;
MilliTimer send;
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
  //EEPROM.write(idAddress,1); //set address
  id = getId();
  Serial.begin(9600);
  Serial.print("slave id:"); 
  Serial.println(id);

  mySerial.begin(9600);
  pinMode(LED,OUTPUT);
}

void loop()
{
  //every sec send a message
  if( send.poll(1000) )
  {
    message.id = id;
    message.status = 5;
    message.voltage = vDivider*AREF/1024*analogRead(voltagePin);
    message.current = iDivider*AREF/1024*analogRead(currentPin);
    message.uptime = millis();
    message.cksum = 0;
    //calculate checksum
    message.cksum = getCheckSum();
    //header
    mySerial.print("\001\002");
    //body
    mySerial.write((const uint8_t *)&message,sizeof(Message));
    flash();
  }
}

void flash()
{
  digitalWrite(LED,ledState);
  ledState = !ledState;
}

byte getId()
{
  return EEPROM.read(idAddress);
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
