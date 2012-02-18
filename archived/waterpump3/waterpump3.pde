#include <TimedAction.h>

/*
hcf solar irrigation
 1/11/09
 Matthew Venn
 
 todo:
 * calibration. 3litres in 53 secs = 56.5 cubic cm per second = 0.0565 l/s
 */


#include <EEPROM.h>
#include <LiquidCrystal.h>

#define DEBUG 0

// rs, rw, enable, d4,d5,d6,d7 
LiquidCrystal lcd(0, 13, 1, 2, 3, 4, 5);

TimedAction measureElectAct = TimedAction(50,measureElect);
TimedAction updateDisplayAct = TimedAction(500,updateDisplay);

//globals
float water;
float power;

long int pumpOnTime;
char systemError;

int avgVoltageReading;
int avgCurrentReading;

float backupWaterLevel;

boolean batteryOK = false;
boolean pumping = false;
boolean waterLevelLow = false;

//errors
#define WATER_LOW 1
#define VOLTAGE_LOW 2

//#define WATER_LOW_DELAY 5000 //how many milliseconds to wait in error condition before reset
//#define VOLTAGE_LOW_DELAY 5000 //how many milliseconds to wait in error condition before reset

//eeprom addresses
#define WATER_STORE 2
#define POWER_STORE 4

// TODO measure these
#define LITRES_PER_SEC 10 //0.2 // this is halved because we run at 8mhz not 16... 0.0565 //5litres in 50 secs: calibrated on 25/2/10
#define WATTS_PER_SEC 100 //0.026 // also halved this value //  3.7A at 12.7 volts = 46.99 watts / 3600 sec

#define LOW_VOLTAGE_LEVEL 500 //10V
#define BACKUP_WATER_LITRES 10 // every x litres pumped, backup our internal state
#define PUMP_ON_CURRENT 620

// pin defs
#define LED_OK 9
#define LED_ERROR 7

#define CURRENT_SENSE A4
#define VOLT_SENSE A2
#define WATER_SENSE 10 //closed circuit when enough water, open when empty
#define PUMP_POWER 8

void setup()
{
  lcd.begin(2, 16);

  lcd.clear();
  lcd.print("HCF solar" );
  lcd.setCursor( 0, 1 );
  lcd.print( "irrigation" );

  //address, value
 // EEPROMWriteInt( WATER_STORE, 0 );
 // EEPROMWriteInt( POWER_STORE, 0 );

  water = (float)EEPROMReadInt( WATER_STORE );
  power = (float)EEPROMReadInt( POWER_STORE );

  //setup globals
  avgVoltageReading = 1;
  avgCurrentReading = 1;

  pumpOnTime = 0;
  systemError = 0;
  backupWaterLevel = water + BACKUP_WATER_LITRES;

  //pin setup
  pinMode( LED_OK, OUTPUT );
  pinMode( LED_ERROR, OUTPUT );

  pinMode( WATER_SENSE, INPUT ); 
  digitalWrite( WATER_SENSE, HIGH );
  pinMode( PUMP_POWER, OUTPUT );

  digitalWrite( PUMP_POWER, LOW );

}

void loop()
{
  //is pump on?
  measureElectAct.check();
  updateDisplayAct.check();

  if( avgVoltageReading < LOW_VOLTAGE_LEVEL )
  {
    systemError = VOLTAGE_LOW;
    digitalWrite( PUMP_POWER, LOW ); //turn off pump
  }
  else if( waterLevelLow )
  {
    systemError = WATER_LOW;
    digitalWrite( PUMP_POWER, LOW );
  }
  else
  {
    systemError = 0;
    digitalWrite( PUMP_POWER, HIGH );
  }

  if( pumping )
  {
    float timePumping = millis() - pumpOnTime;
    pumpOnTime = millis();
    //protect against clock rollover. ignore the moment when it happens.
    if( timePumping > 0 )
    {
      water += timePumping / 1000 * LITRES_PER_SEC;
      power += timePumping / 1000 * WATTS_PER_SEC;
    }
  }
  else
  {
    pumpOnTime = millis();
  }     



  //every x litres, save state
  if( water > backupWaterLevel )
  {
    EEPROMWriteInt( WATER_STORE, (int)water ); //address, value
    EEPROMWriteInt( POWER_STORE, (int)power );
    backupWaterLevel = water + BACKUP_WATER_LITRES;
  }


}

void measureElect()
{
  int voltage = analogRead( VOLT_SENSE );
  int current = analogRead( CURRENT_SENSE );

  avgVoltageReading = ( avgVoltageReading * 0.5 ) + ( voltage * 0.5 );
  avgCurrentReading = ( avgCurrentReading * 0.5 ) + ( current * 0.5 );

  pumping = avgCurrentReading > PUMP_ON_CURRENT ? true : false;
  batteryOK = avgVoltageReading > LOW_VOLTAGE_LEVEL ? true: false;
  waterLevelLow = digitalRead( WATER_SENSE );
}



void updateDisplay()
{
  //heartbeat led on
  digitalWrite( LED_OK, HIGH );  
  lcd.clear();

  /*
  lcd.print( "avgV: " );
   lcd.print( avgVoltageReading );
   
   lcd.setCursor( 0,1 );
   
   lcd.print( "avgI: " );
   lcd.print( avgCurrentReading );
   
   
   
   
   lcd.print( "w:" );
   lcd.print( waterLevelLow ? "low": "ok" );
   
   lcd.print( " p: " );
   lcd.print( avgCurrentReading > PUMP_ON_CURRENT ? "y" : "n" );
   lcd.setCursor(0,1);
   lcd.print( " b: " );
   lcd.print( avgVoltageReading < LOW_VOLTAGE_LEVEL ? "bad" : "ok" );
   */
  switch (systemError) {
  case VOLTAGE_LOW:
    digitalWrite( LED_ERROR, HIGH );
    lcd.print( "ERROR!" );
    lcd.setCursor( 0, 1 );
    lcd.print( "voltage low!" );
    break;
  case WATER_LOW:
    digitalWrite( LED_ERROR, HIGH );
    lcd.print( "ERROR!" );
    lcd.setCursor( 0, 1 );
    lcd.print( "water low!" );
    break;
  default: 
    // if nothing else matches, do the default
    digitalWrite( LED_ERROR, LOW );
    lcd.print( "kW/h: " );
    lcd.print( power / 1000 ); //because power is in watts

    lcd.setCursor(0,1 );
    lcd.print( "L   : " );
    lcd.print( water );

  }

  //heartbeat LED off
  digitalWrite( LED_OK, LOW );    

}

//http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1218921214/0
//thanks to allsystemsgo
//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}






