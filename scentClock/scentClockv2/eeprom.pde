void writeToEeprom()
{
  int addr = 0;
  EEPROM.write(addr++, fanVal);
  EEPROM.write(addr++, holeSize);
  EEPROM.write(addr++, heatVal);
  EEPROM.write(addr++, heatTime);
  EEPROM.write(addr++, ventOpenDelay);
  EEPROM.write(addr++, ventOpenTime);
  Serial.println( "written vals to eeprom OK" );
}                  

void readFromEeprom()
{
  int addr = 0;
  fanVal = EEPROM.read(addr++);
  holeSize = EEPROM.read(addr++);
  heatVal = EEPROM.read(addr++);
  heatTime = EEPROM.read(addr++);
  ventOpenDelay = EEPROM.read(addr++);
  ventOpenTime = EEPROM.read(addr++);

  printVals();
  Serial.println( "loaded OK" );
}

void printVals()
{
  Serial.print( "fan val: " );
  Serial.println( fanVal );

  Serial.print( "hole size: " );
  Serial.println( holeSize );

  Serial.print( "heat val: " );
  Serial.println( heatVal );
  
  Serial.print( "heat time: " );
  Serial.println( heatTime );
  
  Serial.print( "vent open delay: " );
  Serial.println( ventOpenDelay );
  
  Serial.print( "vent open time: " );
  Serial.println( ventOpenTime );
}

