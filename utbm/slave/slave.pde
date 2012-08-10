//#include <JeeLib.h> //for MilliTimer
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


SoftwareSerial mySerial(8,9); //RX,TX

//pin defs
const int LED = 13;
const int voltagePin = A0;
const int currentPin = A1;

//elect defs
const int AREF = 5000.0; //mv
const float vDivider = 0.5;
const float iDivider = 0.5;

//globals
const boolean DEBUG = true;
const int idAddress = 0;
byte id;
//MilliTimer send;
boolean ledState = false;
unsigned long lastTime;

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
 // EEPROM.write(idAddress,2); //set address
  id = getId();
  Serial.begin(9600);
  Serial.print("slave id:"); 
  Serial.println(id);

  mySerial.begin(9600);
  pinMode(LED,OUTPUT);
  
    lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("slave:");
  lcd.print(id);

}

void loop()
{
  //check to see if we need to send anything
  if( mySerial.available() )
  {
    if( mySerial.read() == '\001' )
    {
        debug("sending data");
        lastTime = millis();
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
       
        lcd.setCursor(0, 1);
        // print the number of seconds since reset:
        lcd.print(message.status);
        lcd.print(':');
        lcd.print(millis()/1000);

        flash();
      }
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
