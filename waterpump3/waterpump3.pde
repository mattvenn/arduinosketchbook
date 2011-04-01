/*
hcf solar irrigation
1/11/09
Matthew Venn

todo:
* update low voltage error now that I've changed resisitor?
* voltage was 14v before start.
* heat sink for mosfet
* what about when we turn off the pump's power, when we switch it back on how will we know if it is pumping?
  think this will be ok because the pumpOn() routine will detect. Need to real world check.
* calibration. 3litres in 53 secs = 56.5 cubic cm per second = 0.0565 l/s
*/

#include <MsTimer2.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define DEBUG 0

// rs, rw, enable, d4,d5,d6,d7 
LiquidCrystal lcd(0, 13, 1, 2, 3, 4, 5);
//LiquidCrystal lcd2(3, 0, 3, 5, 4, 7, 6);

//globals
float water;
float power;

char pumpStatus;

long int pumpOnTime;
char systemError;
int avgVoltageReading;
int avgCurrentReading;
int voltageLevel;
float backupWaterLevel;
int voltageDrop;

//errors
#define WATER_LOW 1
#define VOLTAGE_LOW 2

#define WATER_LOW_DELAY 5000 //how many milliseconds to wait in error condition before reset
#define VOLTAGE_LOW_DELAY 5000 //how many milliseconds to wait in error condition before reset

//eeprom addresses
#define WATER_STORE 2
#define POWER_STORE 4

// TODO measure these
#define LITRES_PER_SEC 0.2 // this is halved because we run at 8mhz not 16... 0.0565 //5litres in 50 secs: calibrated on 25/2/10
#define WATTS_PER_SEC 0.026 // also halved this value //  3.7A at 12.7 volts = 46.99 watts / 3600 sec
#define WATER_LEVEL_OK 400 // out of water reads about 20, in water about 800
#define PUMP_VOLTAGE_DROP 30 //4.75 - 4.4 = 0.3V
#define LOW_VOLTAGE_LEVEL 600 //10V
#define BACKUP_WATER_LITRES 10 // every x litres pumped, backup our internal state
#define PUMP_ON_CURRENT 407

// pin defs
#define LED_OK 9
#define LED_ERROR 7
#define PUMP_SENSE 2 //adc
#define WATER_SENSE 1 //adc
#define WATER_SENSE_POWER 10
#define PUMP_POWER 8
#define CURRENT_SENSE 4
void setup()
{
  lcd.begin(2, 16);

  lcd.clear();
  lcd.print("HCF solar" );
  lcd.setCursor( 0, 1 );
  lcd.print( "irrigation" );
  // lcd2.print( "display 2" );
  delay( 500 );
  //address, value


//  EEPROMWriteInt( WATER_STORE, 0 );
//  EEPROMWriteInt( POWER_STORE, 0 );

  water = (float)EEPROMReadInt( WATER_STORE );
  power = (float)EEPROMReadInt( POWER_STORE );

  //setup globals
  avgVoltageReading = 1;
  avgCurrentReading = 1;
  pumpStatus = LOW;
  pumpOnTime = 0;
  systemError = 0;
  backupWaterLevel = 0; //= water + BACKUP_WATER_LITRES;
  
  //pin setup
  pinMode( LED_OK, OUTPUT );
  pinMode( LED_ERROR, OUTPUT );
  pinMode( WATER_SENSE_POWER, OUTPUT );
  pinMode( PUMP_POWER, OUTPUT );
//something wrong with the switching side, plus pump is broken. so turn pump off for now 26/05/2010
//  digitalWrite( PUMP_POWER, HIGH );
digitalWrite( PUMP_POWER, LOW );

  //interrupt setup
  MsTimer2::set(500, updateDisplay); // 500ms period; which is doubled because of half clock speed
  MsTimer2::start();
}

void loop()
{
  //is pump on?
  /*
  if( pumpOn() )
  {
    if ( waterLevelOK() == 1 )
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
    //water level is too low!
    { 
     //display error message and LED
      systemError = WATER_LOW; 
      turnOffPump( WATER_LOW_DELAY ); 
      systemError = 0;
      //reset the pumpOnTime
      pumpOnTime = millis();
    }
  }
  else
  //pump is off, so keep updating the pumpOnTime to keep it accurate
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
  
*/
  //sleep for half second
  delay( 500 ); //this is necessary because the pumpOn routine needs a certain time to determine voltage drop
}

//turn off pump power supply
void turnOffPump( int delayTime )
{
  //turn off pump
  digitalWrite( PUMP_POWER, LOW );    
 //wait for a bit
  delay( delayTime );
  //switch pump power back on
  digitalWrite( PUMP_POWER, HIGH );   
}
      
int pumpOn()
{
  int voltage = analogRead( PUMP_SENSE );
  int current = analogRead( CURRENT_SENSE );
  voltageLevel = voltage;
  //voltageDrop =  avgVoltageReading - voltage;
  // compute running average fr the amplitute
  avgVoltageReading = ( avgVoltageReading * 0.5 ) + ( voltage * 0.5 );
  avgCurrentReading = ( avgCurrentReading * 0.5 ) + ( current * 0.5 );

  if( avgVoltageReading < LOW_VOLTAGE_LEVEL )
  {
      systemError = VOLTAGE_LOW;
      turnOffPump( VOLTAGE_LOW_DELAY );
      return 0;
  }
  else
  {
      systemError = 0;
      digitalWrite( PUMP_POWER, HIGH );
  }
  

  if( avgCurrentReading  >  PUMP_ON_CURRENT )
  {
    pumpStatus = HIGH;
  }
  else
  {
    pumpStatus = LOW;
  }
  return pumpStatus;
}


//this doesn't work anymore 26/05/2010, now always returns 1.
int waterLevelOK()
{
  return 1;
  
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
  lcd.print( "waiting for pump" );
  lcd.setCursor( 0,1 );
  lcd.print( "MV 26/05/2010" );
  /*
  lcd.print( "avgV: " );
  lcd.print( avgVoltageReading );

 lcd.setCursor( 0,1 );
   
  lcd.print( "avgI: " );
  lcd.print( avgCurrentReading );


  lcd.print( voltageLevel );
   lcd.setCursor( 0,1 );
   
   lcd.print( "vd: " );
   lcd.print( voltageDrop );
   lcd.print( " p: " );
   if( pumpStatus == HIGH )
   {
     lcd.print( "on" );
   }
   else
   {
     lcd.print( "off" );
   }
*/  
/*
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
  */
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





