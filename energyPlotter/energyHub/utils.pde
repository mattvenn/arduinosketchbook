
int lastHour = 0, lastDay = 0, lastMinute = 0;
void updateTotals()
{
    Serial.println( freeMemory() );
    }
/*
void updateTotals()
{
  DateTime now = RTC.now();
  //only do this when the hour changes
  if( now.minute() == 0 && now.hour() != lastHour )
  {
    lastHour = now.hour();

    //large numbers seent to cosm
    //4813.083
    //2944.727
    elecKWHH = sumElecWS/1000/3600; 
    gasKWHH = sumGasWS/1000/3600;

    sumElecWS = 0;
    sumGasWS = 0;   
   
    //only do this when the day changes
    if( now.day() != lastDay )
    {
   //    elecKWHD += 
    }
   
  }
  //only do this every 5 minutes
  if( now.minute() % 5 == 0 && now.minute() != lastMinute )
  {
    lastMinute = now.minute();
    int robotEnergy = sumEnergyWS / 1000;
    Serial.print( "sumEnergyWS = " ); Serial.println( sumEnergyWS );
    sendRobotData(robotEnergy);
    sumEnergyWS = 0;
  }
}
*/
/*
//convert gas pulses to kw/h
double convertPulsesToKWH(int pulses)
{
  //from ecotricity gas bill
  //gas pulses are 100ths of a unit
  return pulses * 39.201 * 1.02264 / 3.6 / 100;
}
*/
/*
void doPowerCalculations()
{
  //work out instantaneous power from our current reading P = IV
  elecW = irms * 240;

  //work out electricity and gas energy used in W/S
  if( lastReading > 0 )
  {
    double interval = millis() - lastReading; //in ms
    //need this check to avoid massive power spikes when we lose updates for some time.
    if( interval > 10000 )
      elecWS = 0;
    else
      elecWS = elecW * (interval / 1000);      
  }
  lastReading = millis();

  float gasKWH = convertPulsesToKWH( gasPulses );
  gasWS = (gasKWH * 1000) * 3600;
  Serial.print( "elec ws: " );
  Serial.println( elecWS );
  Serial.print( "gas ws:" );
  Serial.println( gasWS );

//  energyKWS = ( gasWS + elecWS ) / 1000;
 // Serial.print( "energy KWS for robot: " );
 // Serial.println( energyKWS );
  //update totals
  sumEnergyWS += gasWS + elecWS;
  Serial.print( "energy sum WS for robot: " );
  Serial.println( sumEnergyWS );
  sumElecWS += elecWS;
  sumGasWS += gasWS;

  Serial.print( "elec ws this hour: " );
  Serial.println( sumElecWS );
  Serial.print( "gas ws this hour: " );
  Serial.println( sumGasWS );

}
*/
//format the energy numbers into a string to send to pachube
void formatString()
{

  // Convert int/double to string, add it to main string, add csv commas
  // dtostrf - converts a double to a string!
  // strcat  - adds a string to another string
  // strcpy  - copies a string
  strcpy(str,"0,");
  dtostrf(temp,0,1,fstr); 
  strcat(str,fstr);
  
  strcat(str,"\n1,");
  dtostrf(elecW,0,0,fstr);
  strcat(str,fstr);
}

extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
int free_memory;
  
    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
};


