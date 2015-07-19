OneWire  ds(ONE_WIRE_BUS);
#define MAX_DS1820_SENSORS 1
byte addr[8];


bool get_temp()
{
    int HighByte, LowByte, TReading, SignBit;
    float Tc_100;
    byte i, sensor;
    byte data[12];

    ds.reset_search();
    if ( !ds.search(addr)) 
    {
        Serial.print("No more addresses.\n");
        ds.reset_search();
        return false;
    }

    if ( OneWire::crc8( addr, 7) != addr[7]) 
    {
        Serial.println("CRC on address is not valid");
        return false;
    }

    if ( addr[0] == 0x10) 
    {
        Serial.print("Device is a DS18S20 family device.\n");
    }
    else if ( addr[0] == 0x28) 
    {
        Serial.print("Device is a DS18B20 family device.\n");
    }
    else
    {
        Serial.print("Device family is not recognized: 0x");
        Serial.println(addr[0],HEX);
        return false;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);         // start conversion

    //if I had used a pull up then I could poll the line till conversion done

    delay(100);              //time for conversion
    int present = ds.reset(); // device present?
    if(present == 0)
    {
        Serial.println("Device not present after reset");
        return false;
    }
    ds.select(addr);

    ds.write(0xBE);         // Read Scratchpad

    // we need 9 bytes
    for ( i = 0; i < 9; i++) 
        data[i] = ds.read();

    // check data integrity
    if ( OneWire::crc8( data, 8) != data[8]) 
    {
        Serial.println("CRC on data is not valid");
        return false;
    }
      
    // otherwise data is good
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000;  // test most sig bit
    if(SignBit) // negative
    {
        TReading = (TReading ^ 0xffff) + 1; // 2's comp
    }
    Tc_100 = (TReading*100/2);    
    temp = Tc_100 / 100;

    if(temp == 85)
    {
        Serial.println("conversion didn't happen");
        return false;
    }
    return true;
}


