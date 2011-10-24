int currentOffset;

void calibrate()
{
  lcd.clear();
  lcd.print("cal:");

  //should check that turbine voltage is 0 first. TODO
  int current = 0;
  for( int i = 0; i < 10 ; i ++ )
  {
    lcd.print(".");
    current += analogRead(PIN_current);
    delay(100);
  }
  currentOffset = current / 10;
}

void initialiseAvgArray( int array[], int arrayLength )
{
  for( int i = 0; i < arrayLength ; i ++ )
    array[i] = 0;
}

void updateElectrical()
{
  byte delayTime = 10;

  //read the pin, wait and read again for more accurate ADC
  analogRead(PIN_battVoltage);
  delay(delayTime);
  rawBattVoltage = analogRead(PIN_battVoltage);

  analogRead(PIN_current);
  delay(delayTime);
  rawCurrent = analogRead(PIN_current);

  analogRead(PIN_turbineVoltage); 
  delay(delayTime);
  rawTurbineVoltage = analogRead(PIN_turbineVoltage); 

  if( rawCurrent < currentOffset )
    rawCurrent = 0;
  else
    rawCurrent -= currentOffset;    

  //conversions
  //905 = 12.16v
  battVoltage = rawBattVoltage / 74.42;

  //529 = 12.15v
  turbineVoltage = rawTurbineVoltage / 43.53;
  avgCurrent = getAverage( rawCurrent, rawCurrentArray, currentArrayLength );
  current = avgCurrent / 110.0;

  dumpPercent = (255 - pwmDumpLoad ) / 255 * 100;
}
#ifdef SERIAL_LOG
//spit out all the useful values
void serialLog()
{
  Serial.print(Setpoint);   
  Serial.print(" ");
  Serial.print(battVoltage);   
  Serial.print(" ");
  Serial.print(pwmDumpLoad);   
  Serial.print(" ");  
  Serial.print(turbineVoltage);
  Serial.print(" ");  
  Serial.print(current);
  Serial.println("");
}
#endif
void checkBattery()
{
  if( battVoltage < LOW_BATTERY_VOLTAGE )
  {
    digitalWrite( PIN_phoneChargerSwitch, LOW );

    errors[ERR_LOW_BATTERY] = 1;
  }
  else
  {
    digitalWrite( PIN_phoneChargerSwitch, HIGH );
    errors[ERR_LOW_BATTERY] = 0;
  }
}

int getAverage( int val, int array[], int arrayLength )
{
  int total = 0;
  for( int i = 1; i < arrayLength ; i ++ )
  {
    total += array[i];
    array[i-1] = array[i];
  }
  array[arrayLength-1] = val;
  total += val;
  return total /= arrayLength;
}

