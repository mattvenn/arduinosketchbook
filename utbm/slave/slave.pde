/*
todo
test!
*/
//#include <JeeLib.h> //for MilliTimer

#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
//useful shared stuff for master and slave
#include "fuelcell.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int RX = 8;
const int TX = 9;

long int lastComms;
const int badComms = 5000; //5 secs until we display a bad comms symbol (- for bad, + for good)

SoftwareSerial masterSerial(RX,TX); //RX,TX
SoftwareSerial fuelcellSerial(11,12);  // define pins for rs232 comms with Hymera

//pin defs
const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;

//globals
const boolean DEBUG = true;
const int idAddress = 0;
byte id;
boolean ledState = false;
long int timer;

void setup()  
{
 // EEPROM.write(idAddress,2); //set address
  id = getId();
  initialiseMessage(); //blank all the data, stops the LCD showing junk
  Serial.begin(9600);
  Serial.print("slave id:"); 
  Serial.println(id);

  masterSerial.begin(9600);
  setupFuelcellSerial();
  pinMode(LED,OUTPUT);

  //not sure if needed
  pinMode(RX,INPUT);

  lcd.begin(16, 2);
  displayStartScreen();
  delay(2000);

}

void loop()
{
  //update fuelcell serial
 // if( millis() - timer > 2000 )
  {
   // Serial.println( "update fuel cell data" );
    timer = millis();
    updateFuelCellStatus();
    //update display and print data on serial
    displayLCDFuelCellStatus();
    printFuelCellStatus();
  }
  //sends data to master, times out
  sendData();  

}

void displayStartScreen()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( "Arcola Energy");
  lcd.setCursor(0,1);
  lcd.print( "lab systems");
}

void displayLCDSend()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print( "sending data" );
  lcd.setCursor(0, 1);
  lcd.print( "id " );
  lcd.print( id );
}

void displayLCDFuelCellStatus()
{
  lcd.clear();
  lcd.setCursor(0,0);
  //if all OK, show some values
  if( message.fuelcellStatus == 0 )
  {
    lcd.print( message.fuelcellStackI * message.fuelcellStackV, 1 );
    lcd.print( "W" );
    lcd.setCursor(8,0);
    lcd.print( message.fuelcellStackT, 1 );
    lcd.print( "C" );
    lcd.setCursor(15,0);
    lcd.print( millis() - lastComms > badComms ? "-" : "+" );
    lcd.setCursor(0, 1);
    lcd.print( message.fuelcellStackV, 1 );
    lcd.print( "V" );
    lcd.setCursor(8, 1);
    lcd.print( message.fuelcellStackI, 1 );
    lcd.print( "A" );
  }
  //otherwise show the error
  else
  {
    lcd.print( "fuelcell error:" );
    lcd.setCursor(0,1);
    printStatus(message.fuelcellStatus);
  }
}

void sendData()
{
  int count = 0;
  //wait for master to ask for data
  while( digitalRead(RX) == LOW )
  {
    if( count ++ > 10 )
    {
      Serial.println( "giving up on master sig" );
      return;
    }
    delay(100);
  }
  
  Serial.println( "got master sig" );
  //wait for line to go low
  while( digitalRead(RX) ) {;;}
  
  message.id = id;
  message.uptime = millis();
  message.cksum = 0;
  //calculate checksum
  message.cksum = getCheckSum();
  //header
  masterSerial.print("\001\002");
  //body
  masterSerial.write((const uint8_t *)&message,sizeof(Message));
  flash();
  displayLCDSend();
  lastComms = millis();
}

void debug(const char * msg )
{
  if( DEBUG )
    Serial.print(msg);
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


