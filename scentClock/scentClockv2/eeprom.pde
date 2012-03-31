
void writeToEeprom()
{
  int addr = 0;
  EEPROMWriteInt(addr+=2, fanVal);
  EEPROMWriteInt(addr+=2, heatVal);
  EEPROMWriteInt(addr+=2, heatTime);
  EEPROMWriteInt(addr+=2, ventOpenDelay);
  EEPROMWriteInt(addr+=2, ventOpenTime);
  EEPROMWriteInt(addr+=2, switchInterval);
  Serial.println( "written vals to eeprom OK" );
  
}                  

void readFromEeprom()
{
  int addr = 0;
  fanVal = EEPROMReadInt(addr+=2);
  heatVal = EEPROMReadInt(addr+=2);
  heatTime = EEPROMReadInt(addr+=2);
  ventOpenDelay = EEPROMReadInt(addr+=2);
  ventOpenTime = EEPROMReadInt(addr+=2);
  switchInterval = EEPROMReadInt(addr+=2);

  printVals();
  Serial.println( "loaded OK" );
}

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


