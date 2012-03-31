int serReadInt()
{
 byte upper = Serial.read();
 byte lower = Serial.read();
 int x = (upper<<8) | lower; //Reassemble the number
 return x;
}

void printVals()
{
 

  Serial.print( "heat val: " );
  Serial.println( heatVal, DEC );
  
  Serial.print( "heat time: " );
  Serial.println( heatTime, DEC );
  
  Serial.print( "vent open delay: " );
  Serial.println( ventOpenDelay, DEC );
  
  Serial.print( "vent open time: " );
  Serial.println( ventOpenTime, DEC );
  
   Serial.print( "fan val: " );
  Serial.println( fanVal, DEC );
  
    Serial.print( "switch interval: " );
  Serial.println( switchInterval, DEC );
}

