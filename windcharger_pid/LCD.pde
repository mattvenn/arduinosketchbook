int lcdScreen = 0;

void flashLED()
{
  //update LED too
  statusLED = ! statusLED;
  digitalWrite( PIN_ledGreen, statusLED );
}

void updateLCD()
{
  
  lcd.clear();
  lcd.setCursor(0,0);
  
  //page through screens
  if( lcdScreen ++ >= 2 )
    lcdScreen = 0;
  
  lcdScreen = 2;
  
  switch( lcdScreen )
  {
    case 0: //errors
      lcd.print( "errs: " );
      printErrors();
      break;
    case 1: //battery status
 //     if( rawCurrent > CURRENT_CHARGING_VALUE )
   //   {
    //    lcd.print( "charging" );
    //  }
   //   else
      {
        lcd.print( "discharging" );
      }
      
      lcd.setCursor(0,1);
      for( int i = 0; i < 16; i ++ )
      {
        lcd.write( history[i] );
      } 
      break;
      
    case 2: //other vars
      lcd.print( "b:" );
      dtostrf( battVoltage, 4, 1, buff );
      lcd.print( buff );
      lcd.print("V");
      
      lcd.setCursor(8,0);
      lcd.print("t:" );
      dtostrf( turbineVoltage, 4, 1, buff );
      lcd.print( buff );
      lcd.print("V");

      lcd.setCursor(0,1);
      lcd.print( "d ");
      dtostrf( dumpPercent, 4, 0, buff );
      lcd.print( buff );
      lcd.print( "%" );
      
      lcd.setCursor(8,1);
      lcd.print("I:");
      dtostrf( current, 4, 1, buff );
      lcd.print(buff);
      lcd.print("A");
     
      break;
  }
}


void addToHistory( )
{
  for( int i = 0; i < 15 ; i ++ )
  {
    history[ i ] = history[ i + 1 ];
  }
  int histVal = map( battVoltage, 10, 15, 0, 7 );
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
