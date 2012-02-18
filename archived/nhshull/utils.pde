int getAverage( unsigned int val, unsigned int array[], unsigned int arrayLength )
{
  long int total = 0;
  for( int i = 1; i < arrayLength ; i ++ )
  {
    total += array[i];
    array[i-1] = array[i];
  }
  array[arrayLength-1] = val;
  total += val;
  return total /= arrayLength;
}

void initialiseAvgArray( unsigned int array[], unsigned int arrayLength )
{
  for( int i = 0; i < arrayLength ; i ++ )
    array[i] = 0;
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


