//print the status information onto the LCD
void updateStatus( int newStatus)
{
  systemStatus = newStatus;
  updateDisplay();
}

void updateDisplay()
{
  lcd.clear();
  getDateTime();
  if( systemError != 0)
  {
    displayError();
  }
  else
  {
    lcd.print( "status: " );
    switch( systemStatus ) 
  {
    case USB_READY:
    lcd.print( "usb ready" );
    break;
    case RUNNING:
    lcd.print( "running" );
    break;
  case SETUP_USB:
    lcd.print( "wait on USB" );
    break;
  case WAIT_ON_FUELCELL:
    lcd.print( "wait on data" );
    break;
  case LOGGING:
    lcd.print( "logging" );
    break;
  case SHUTDOWN:
    lcd.clear();
    lcd.print( "shutting down..." );
    break;
  }
    lcd.setCursor( 0, 1 ); 
    for( int i = 0; i < 8 ; i ++ )
    {
      lcd.write( dateString.charAt(i) );
    }
if( usbReadyToLog )
{
    lcd.print( " log #" );
    lcd.print( useID );
} 

    
    lcd.setCursor( 0, 2 );
    switch( powerFrom )
    {
      case BATTERY:
      lcd.print( "using batt: " );
      lcdPrintDouble( battLevel / BATT_ADC_RATIO, 1 );
      lcd.print( "V" );
      break;
      case MAINS:
      lcd.print( "charge batt: " );
      lcdPrintDouble( battLevel / BATT_ADC_RATIO, 1 );
      lcd.print( "V" );
      break;
      case FUELCELL:
      lcd.print( "using fuelcell:" );
      lcdPrintDouble( fuelcell.stackV / 10, 1 );
      lcd.print( "V" );
      break;
    }
     
    
   
//    lcd.print( " Amps:" );
 //   lcdPrintDouble( fuelcell.current, 1 );
    
  }
}
/*
print doubles to the LCD
routine from http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1207226548/13#13
*/
void lcdPrintDouble( double val, byte precision){
  // prints val on a ver 0012 text lcd with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  if(val < 0.0){
    lcd.print('-');
    val = -val;
  }

  lcd.print (int(val));  //prints the int part
  if( precision > 0) {
    lcd.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
  mult *=10;

    if(val >= 0)
 frac = (val - int(val)) * mult;
    else
 frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
 padding--;
    while(  padding--)
 lcd.print("0");
    lcd.print(frac,DEC) ;
  }
}
