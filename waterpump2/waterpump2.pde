/*
hcf solar irrigation
1/11/09
Matthew Venn

todo:

* still counts time if in error state
* refactor error stuff
*/

#include <MsTimer2.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define DEBUG 0

// rs, rw, enable, d4,d5,d6,d7 
LiquidCrystal lcd(0, 13, 1, 2, 3, 4, 5);
//LiquidCrystal lcd2(3, 0, 3, 5, 4, 7, 6);

//globals
unsigned int water;
unsigned int power;
unsigned int waterUsed;
unsigned int powerUsed;
char pumpStatus;

long int pumpOnTime;
long int timer;
char startTimer;
char systemError;
int avgVoltageReading;
int voltageLevel ;

//errors
#define WATER_LOW 1
#define VOLTAGE_LOW 2

#define WATER_LOW_DELAY 5000 //how many milliseconds to wait in error condition before reset

//eeprom addresses
#define WATER_STORE 2
#define POWER_STORE 0

// TODO measure these
#define LITRES_PER_SEC 10.5
#define WATTS_PER_SEC 10.2
#define WATER_LEVEL_OK 400 // out of water reads about 20, in water about 800
#define PUMP_VOLTAGE_DROP 20 //0.3V
#define LOW_VOLTAGE_LEVEL 730 //10V

// pin defs
#define LED_OK 9
#define LED_ERROR 7
#define PUMP_SENSE 2 //adc
#define WATER_SENSE 1 //adc
#define WATER_SENSE_POWER 10
#define PUMP_POWER 8

void setup()
{
  lcd.begin(2, 16);
  lcd.home();

  lcd.clear();
  lcd.print("HCF solar" );
  lcd.setCursor( 0, 1 );
  lcd.print( "irrigation" );
  // lcd2.print( "display 2" );
  delay( 500 );
  //address, value
  //EEPROMWriteInt( WATER_STORE, 0 );
  //EEPROMWriteInt( POWER_STORE, 0 );
  water = EEPROMReadInt( WATER_STORE );
  power = EEPROMReadInt( POWER_STORE );

  //setup globals
  waterUsed = 0;
  powerUsed = 0;
  timer = 0;
  startTimer = 1;
  avgVoltageReading = 500;
  pumpStatus = LOW;
  systemError = 0;

  //pin setup
  pinMode( LED_OK, OUTPUT );
  pinMode( LED_ERROR, OUTPUT );
  pinMode( WATER_SENSE_POWER, OUTPUT );
  pinMode( PUMP_POWER, OUTPUT );
  digitalWrite( PUMP_POWER, HIGH );

  //interrupt setup
  MsTimer2::set(500, updateDisplay); // 500ms period
  MsTimer2::start();

}

void loop()
{
  //wake
  long int timePumping = 0;
  //is pump on?
  if( pumpOn() )
  {
    if( startTimer == 1 )
    {
      pumpOnTime = millis();
      startTimer = 0;
    }

    if ( waterLevelOK() == 1 )
    {
      timePumping = millis() - pumpOnTime;
      waterUsed = timePumping / 1000; // * LITRES_PER_SEC;
      powerUsed = timePumping / 1000; // * WATTS_PER_SEC;
    }
    else
    {  
      //turn off pump power supply
      digitalWrite( PUMP_POWER, LOW );   
      //display error message and LED
      systemError = WATER_LOW;
      //wait for 1 minute before resetting
      delay( WATER_LOW_DELAY );
      //switch pump power back on
      digitalWrite( PUMP_POWER, HIGH );   
      systemError = 0;
    }
  }
  else
  {
    if( startTimer == 0 )
    {
      digitalWrite( LED_ERROR, LOW );
      startTimer = 1;
      //sleep - TODO - actually sleep
      //delay( 300 );
      water += waterUsed;
      waterUsed = 0;
      power += powerUsed;  
      powerUsed = 0;
      //update eeprom 
      EEPROMWriteInt( WATER_STORE, water ); //address, value
      EEPROMWriteInt( POWER_STORE, power );
    }

  } 

  //sleep for half second
  delay( 500 ); //this is necessary because the pumpOn routine needs a certain time to determine voltage drop


}

int pumpOn()
{
  int voltage = analogRead( PUMP_SENSE );
  voltageLevel = voltage;
  if( voltage < LOW_VOLTAGE_LEVEL )
  {
      systemError = VOLTAGE_LOW;
      digitalWrite( PUMP_POWER, LOW );
  }
  else
  {
      systemError = 0;
      digitalWrite( PUMP_POWER, HIGH );
  }
  
  int voltageDrop =  avgVoltageReading - voltage;
  // compute running average fr the amplitute
  avgVoltageReading = ( avgVoltageReading * 0.5 ) + ( voltage * 0.5 );

  //if we find a big enough positive voltage drop then pump is on
  if( voltageDrop > PUMP_VOLTAGE_DROP )
  {
    pumpStatus = HIGH;
  }
  //if we find a big enough voltage rise then pump stopped
  else if( -1 * voltageDrop > PUMP_VOLTAGE_DROP )
  {
    pumpStatus = LOW;
  }

  return pumpStatus;
}



int waterLevelOK()
{
  digitalWrite( WATER_SENSE_POWER, HIGH );
  int waterLevel = analogRead( WATER_SENSE );
  digitalWrite( WATER_SENSE_POWER, LOW );
  if( waterLevel > WATER_LEVEL_OK )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


void updateDisplay()
{
  //heartbeat led on
  digitalWrite( LED_OK, HIGH );  

  lcd.clear();
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
    lcd.print( "power: " );
    lcd.print( power + powerUsed );
    lcd.print( " kW/h");
    lcd.setCursor(0,1 );
    lcd.print( "water: " );
    lcd.print( water + waterUsed );
    lcd.print( " L" );
/*
    if( pumpStatus == HIGH )
    {
       lcd.print( " 1" );
    }
    else
    {
       lcd.print( " 0" );
    }
  */  
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





