
/* 
 todo:
 + servo hole fine tuning
 - timings
 - serial comms
 */
#include <EEPROM.h>
#include <MsTimer2.h>
#include <Servo.h> 
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t


#define HEATER1 3
#define HEATER2 5
#define HEATER3 6
#define STATUS_LED 8
#define FAN 9
#define SWITCH 2


int heatVal = 50; //this is inverted
int heatTime = 5;
int ventOpenDelay = 2;
int ventOpenTime = 4;
int holeSize = 20;
int fanVal;
int rawTemp;



void setup()
{
  pinMode(STATUS_LED, OUTPUT );
  
  flashLED(); 
  flashLED();
  
  pinMode(SWITCH, INPUT );
  digitalWrite( SWITCH, HIGH ); 

  analogWrite( HEATER1, 0 );
  analogWrite( HEATER2, 0 );
  analogWrite( HEATER3, 0 );

  analogWrite( FAN, 0 );

  Serial.begin(9600);
  Serial.println( "scent clock" );

  Serial.println( "loading vals from eeprom...");
  readFromEeprom();

}

void flashLED()
{
  digitalWrite( STATUS_LED, HIGH);
  delay(100);
  digitalWrite( STATUS_LED, LOW );
}
void readParams()
{
  char command = Serial.read();
  if( command == 'A' )
  {
    delay(100);

    //all these are byte values from 0 to 255
    fanVal = Serial.read();
    holeSize = Serial.read();
    heatVal = Serial.read();
    heatTime = Serial.read();    
    ventOpenDelay = Serial.read();
    ventOpenTime = Serial.read();

    //write them all to eeprom
    writeToEeprom();

    flashLED();    

  }
  else if( command == 'B' )
  {
    printVals();
    flashLED();

  }
  else if( command == 'C' )
  {
    flashLED();
    releaseScent();

  }
  else
  {
    //flush serial port
    while( Serial.available() )
    {
      Serial.read();
    }
  }
}
void loop()
{
  if( digitalRead( SWITCH ) == LOW )
  {
    Serial.println( "switch pressed" );
    releaseScent();
  }
  if( Serial.available() )
  {
    readParams();
  }
}

//broken wrt timings
void releaseScent()
{
  Serial.println( "releasing scent" );
  Serial.print( "turning on heater: " );
  Serial.println( heatVal );
  analogWrite( HEATER1, heatVal );
  int time = 0;
  boolean heatOn = true;
  boolean ventOpen = false;
  while( heatOn || ventOpen)
  {
    if( time == heatTime )
    {
      Serial.println( "turning off heater" );
      analogWrite( HEATER1, 0 );
      heatOn = false;
    }
    if( time == ventOpenDelay )
    {
      if( fanVal != 0 )
      {
        Serial.print( "turning on fan: " );
        Serial.println( fanVal );
        analogWrite( FAN, convertFanVal(fanVal) );
        ventOpen = true;
      }
    }
    if( time == ventOpenDelay + ventOpenTime )
    {    
      Serial.println( "turning off fan" );
      analogWrite( FAN, 0 ); 
      ventOpen = false;
    }
    delay(1000);
    Serial.print( "t=" );
    time ++;
    Serial.println( time, DEC );

  }
  Serial.println( "finished" );
}

int convertFanVal( int fan )
{
  return fan += 155;
}

