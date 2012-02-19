//convert gas pulses to kw/h
double convertPulsesToKWH(int pulses)
{
  //from ecotricity gas bill
    //gas pulses are 100ths of a unit
    return pulses * 39.201 * 1.02264 / 3.6 / 100;
}

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

void formatTotalString()
{

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
    
    dtostrf(gas,0,3,fstr);
    strcat(str,fstr);
    strcat(str,"\n2,");    
    
    dtostrf(temp,0,3,fstr);
    strcat(str,fstr);
    strcat(str,"\n3,");    
    
    dtostrf(irms,0,3,fstr);
    strcat(str,fstr);
    strcat(str,"\n4,");
    
    dtostrf(power,0,3,fstr);
    strcat(str,fstr);
    strcat(str,"\n5,");
    
    dtostrf(totalElecKWH,0,3,fstr); 
    strcat(str,fstr);
    strcat(str,"\n6,");
    
    dtostrf(totalGasKWH,0,3,fstr);
    strcat(str,fstr);       
}
