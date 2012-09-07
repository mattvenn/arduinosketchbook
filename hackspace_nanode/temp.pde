/* DS18S20 Temperature chip i/o */

#include <OneWire.h>
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;


OneWire  ds(3);  // on pin 3
#define MAX_DS1820_SENSORS 1
byte addr[8];
void setupTherm(void) 
{
  if (!ds.search(addr)) 
  {
    ds.reset_search();
    delay(250);
    return;
  } 
}

void getTemp()
{
  byte i, sensor;
  byte present = 0;
  byte data[12];


    if ( OneWire::crc8( addr, 7) != addr[7]) 
    {
      Serial.println("CRC is not valid");
      return;
    }

    if ( addr[0] != 0x10) 
    {
      
      Serial.println("Device is not a DS18S20 family device.");
      return;
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
    Tc_100 = (TReading*100/2);    

    Whole = Tc_100 / 100;  // separate off the whole and fractional portions
    Fract = Tc_100 % 100;

    sprintf(tempStr, "%c%d.%d",SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);  
    //Serial.print( "new temp: " );
    //Serial.println( tempStr );
}

