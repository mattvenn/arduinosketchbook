int lcdScreen = 0;

void updateLCD()
{
  //update LED too
  statusLED = ! statusLED;
  digitalWrite( PIN_ledGreen, statusLED );

  lcd.clear();
  lcd.setCursor(0,0);
  
  //page through screens
  if( lcdScreen ++ > 2 )
    lcdScreen = 0;
    
  switch( lcdScreen )
  {
    case 0: //errors
      lcd.print( "errs: " );
      printErrors();
  
    case 1: //battery status
      if( rawCurrent > CURRENT_CHARGING_VALUE )
      {
        lcd.print( "charging" );
      }
      else
      {
        lcd.print( "discharging" );
      }
      
      lcd.setCursor(0,1);
      for( int i = 0; i < 16; i ++ )
      {
        lcd.write( history[i] );
      } 
     
    case 2: //other vars
      lcd.print( "bat:" );
      lcd.print( rawBattVoltage );
      lcd.print("V");
      lcd.print( " dump ");
      lcd.print( pwmDumpLoad );

      lcd.setCursor(0,1);
      lcd.print( "turb " );
      lcd.print( rawTurbineVoltage );
      lcd.print( "V" );
      lcd.print( "cur ");
      lcd.print( rawCurrent );
      lcd.print( "A" );
      
  }
}

void addToHistory( )
{
  for( int i = 0; i < 15 ; i ++ )
  {
    history[ i ] = history[ i + 1 ];
  }
  int histVal = map( rawBattVoltage, 500, 700, 0, 7 );
  if( histVal > 7 )
    histVal = 7;
  if( histVal < 0 )
    histVal = 0;
  history[ 15 ] = histVal;
}

void createBars()
{
  byte bar[8];
  for( int barNum = 0; barNum < 8 ; barNum ++ )
  {
    for( int row = 0; row < 8; row ++ )
    {
      if( row < barNum )
      {
        bar[7-row] = 0xFF;
      }
      else
      {
        bar[7-row] = 0x00;
      }
    } 
    lcd.createChar(barNum, bar);
  }
  for( int i = 0; i < 16; i ++ )
    history[i] = 0;
}
