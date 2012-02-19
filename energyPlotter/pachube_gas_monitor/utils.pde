
//print the time and update the minutes global
void printRTCTime()
{
  DateTime now = RTC.now();
  Serial.print( "RTC: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  //update global minutes 
  minutes = now.hour() * 60 + now.minute();

}

int lastHour = 0;

void updateTotals()
{
  DateTime now = RTC.now();
  //only do this when the hour changes
  if( now.minute() == 0 && now.hour() != lastHour )
  {
    lastHour = now.hour();

    elecKWH = sumElecWS/1000/3600; 
    gasKWH = sumGasWS/1000/3600;

    sumElecWS = 0;
    sumGasWS = 0;   
  }
}

//convert gas pulses to kw/h
double convertPulsesToKWH(int pulses)
{
  //from ecotricity gas bill
  //gas pulses are 100ths of a unit
  return pulses * 39.201 * 1.02264 / 3.6 / 100;
}


void doPowerCalculations()
{
  //work out instantaneous power from our current reading P = IV
  elecW = irms * 240;

  //work out electricity and gas energy used in W/S
  if( lastReading > 0 )
  {
    double interval = millis() - lastReading; //in ms
    elecWS = elecW * (interval / 1000);      
  }
  lastReading = millis();

  float gasKWH = convertPulsesToKWH( gasPulses );
  gasWS = (gasKWH * 1000) * 3600;
  Serial.print( "elec ws: " );
  Serial.println( elecWS );
  Serial.print( "gas ws:" );
  Serial.println( gasWS );

  energyKWS = ( gasWS + elecWS ) / 1000;
  Serial.print( "energy KWS for robot: " );
  Serial.println( energyKWS );
  //update totals
  sumElecWS += elecWS;
  sumGasWS += gasWS;

  Serial.print( "elec ws this hour: " );
  Serial.println( sumElecWS );
  Serial.print( "gas ws this hour: " );
  Serial.println( sumGasWS );

}

//format the energy numbers into a string to send to pachube
void formatString()
{

  // Convert int/double to string, add it to main string, add csv commas
  // dtostrf - converts a double to a string!
  // strcat  - adds a string to another string
  // strcpy  - copies a string
  strcpy(str,"0,");

  dtostrf(battv,0,3,fstr); 
  strcat(str,fstr);
  strcat(str,"\n1,");

  dtostrf(gasPulses,0,3,fstr);
  strcat(str,fstr);
  strcat(str,"\n2,");    

  dtostrf(temp,0,3,fstr);
  strcat(str,fstr);
  strcat(str,"\n3,");    

  dtostrf(irms,0,3,fstr);
  strcat(str,fstr);
  strcat(str,"\n4,");

  dtostrf(elecW,0,3,fstr);
  strcat(str,fstr);
  strcat(str,"\n5,");

  dtostrf(elecKWH,0,3,fstr); 
  strcat(str,fstr);
  strcat(str,"\n6,");

  dtostrf(gasKWH,0,3,fstr);
  strcat(str,fstr);       
}


