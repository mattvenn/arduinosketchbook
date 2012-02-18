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

//format the energy numbers into a string to send to pachube
void formatString()
{
  
    // Convert int/double to string, add it to main string, add csv commas
    // dtostrf - converts a double to a string!
    // strcat  - adds a string to another string
    // strcpy  - copies a string
    strcpy(str,"");
    
    dtostrf(battv,0,3,fstr); 
    strcat(str,fstr);
    strcat(str,",");
    
    dtostrf(gas,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");    
    
    dtostrf(temp,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");    
    
    dtostrf(irms,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");
    
    dtostrf(power,0,3,fstr);
    strcat(str,fstr);
    
}
