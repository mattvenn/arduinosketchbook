/* DS18S20 Temperature chip i/o */
//#include <JeeLib.h>
#include <PID_v1.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 7, 6, 5, 4, 3);

#include <OneWire.h>

OneWire  ds(2);  // on pin 3
#define MAX_DS1820_SENSORS 1
byte addr[8];
double printTimer,pidTimer;
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,15,15,1, REVERSE);

void setup()
{
Serial.begin(9600);
    Serial.println( "started" );
 
  delay(200);
      setupTherm();
      
      Input = getTemp();
  Setpoint = 30000;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("pid bed heater");

}
void loop()
{
  if(millis() - printTimer > 500)
  {
  printTimer = millis();
  Serial.print( "temp: " );
Serial.println( Input );
Serial.print( "mosfet output:" );
  Serial.println( Output );
  Serial.print("target temp: ");
  Serial.println( Setpoint );
  lcd.clear();
  lcd.setCursor(0,0);
//  lcd.print("b" );
  lcd.print(Input/1000);
  lcd.print("C");
  lcd.print( " > ");
  lcd.print( Setpoint/1000);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("heater:");
  lcd.print( (255 - Output ) / 255 * 100 );
  lcd.print( "%" );
}
if(millis() - pidTimer > 100)
{
  pidTimer = millis();
  Setpoint = map( analogRead(A0), 0, 1024, 20000,130000);
  Input = getTemp();
  myPID.Compute();
  
  analogWrite(10,Output);
}
}


void setupTherm(void) 
{
  if (!ds.search(addr)) 
  {
    ds.reset_search();
    delay(250);
    return;
  } 
}

double getTemp()
{
int HighByte, LowByte, TReading, SignBit;
double Tc_100;

  byte i, sensor;
  byte present = 0;
  byte data[12];


    if ( OneWire::crc8( addr, 7) != addr[7]) 
    {
      Serial.println("CRC is not valid");
      return 0;
    }

    else if ( addr[0] == 0x28) {
      //Serial.print("Device is a DS18B20 family device.\n");
  }

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) 
    {           // we need 9 bytes
      data[i] = ds.read();
    }

    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000;  // test most sig bit
    if (SignBit) // negative
    {
      TReading = (TReading ^ 0xffff) + 1; // 2's comp
    }
    //Serial.println(TReading);
    Tc_100 = TReading;
    Tc_100 *= 50;

   // Whole = Tc_100 / 100;  // separate off the whole and fractional portions
    //Fract = Tc_100 % 100;

   // sprintf(tempStr, "%c%d.%d",SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);  

    //Serial.print( "new temp: " );
    //Serial.println( tempStr );
    return Tc_100;
}

