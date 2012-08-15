//#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
//useful shared stuff for master and slave
#include "fuelcell.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


SoftwareSerial masterSerial(8,9); //RX,TX
SoftwareSerial fuelcellSerial(11,12);  // define pins for rs232 comms with Hymera

//pin defs
const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;

//globals
const boolean DEBUG = true;
const int idAddress = 0;
byte id;
//MilliTimer send;
boolean ledState = false;
unsigned long lastTime;

void setup()  
{
  // EEPROM.write(idAddress,2); //set address
  id = getId();
  Serial.begin(9600);
  Serial.print("slave id:"); 
  Serial.println(id);

  masterSerial.begin(9600);
  setupFuelcellSerial();
  pinMode(LED,OUTPUT);

  lcd.begin(16, 2);
  

}

void displayFuelCellStatus()
{
  //lcd.setCursor(8, 0);
  //lcd.print "
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("id:");
  lcd.print(id);
  lcd.print(" ");
  lcd.print( message.fuelcellStackT );
  lcd.print( "C" );
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print( message.fuelcellStackV );
  lcd.print( "V " );
  lcd.print( message.fuelcellOutputCurrent );
  lcd.print( "A" );
}
void loop()
{
  //update fuelcell serial
  updateFuelCellStatus();

  printFuelCellStatus();
  displayFuelCellStatus();

  sendData();
  
}

void sendData()
{
  //wait for master to ask for data
  masterSerial.listen();
  while (masterSerial.available() != 1 )
  {
    delay (10); 
  }
  if( masterSerial.read() == '\001' )
  {
    debug("sending data\n");

    lastTime = millis();
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
  }
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
  if( ledState )
    lcd.cursor();
  else
    lcd.noCursor();
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


