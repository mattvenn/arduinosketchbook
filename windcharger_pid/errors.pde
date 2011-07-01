
void setupErrors()
{
  
  for( int errorNum = 0; errorNum < numErrors ; errorNum ++ )
  {
    errors[errorNum] = false;
  }
}

void printErrors()
{
    if( errors[ERR_LOW_BATTERY] )
     lcd.print( "Batt Low" ); 
}
