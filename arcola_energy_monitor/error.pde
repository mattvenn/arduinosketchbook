void displayError()
{
  lcd.clear();
  lcd.setCursor( 0, 0 );
  lcd.print( "status: error" );
  lcd.setCursor( 0, 1 );
  switch( systemError ) 
  {
  case USB_DEVICE_FAILED:
    lcd.print( "giving up on usb" );
    usbReadyToLog = false;
    break;
  case USB_NOT_READY:
    lcd.print( "wait on usb" );
    lcd.setCursor( 0, 2 );
    lcd.print( "plug disk in" );
    usbReadyToLog = false;
    break;
  case USB_COMMAND_FAIL:
    lcd.print( "USB command failed:" );
    lcd.setCursor( 0, 2 );
    lcd.print( usbString );
    usbReadyToLog = false;

    break;
  case USB_DEVICE_REMOVED:
    lcd.print( "USB stick removed" );
    usbReadyToLog = false;
    break;
  case FUELCELL_NOT_READY:
    lcd.print( "fuel cell" );
    lcd.setCursor( 0,2 );
    lcd.print( "not sending data" );
    break;
  case LOW_BATTERY:
  lcd.print( "battery too low!" );
  lcd.setCursor( 0,2 );
  lcd.print( "shutdown in " );
  lcd.print( 15 - lowBatteryTimeOut * 5 );
  lcd.print( " secs" );
  break;
  default :
    lcd.print( "unknown" );
    break;
  }
  //time to read the error
    delay( 1000 );
}


void error( int errorType )
{
    digitalWrite( STATUS_OK, LOW );
    digitalWrite( STATUS_BAD, HIGH);
  systemError = errorType;
//  writeErrorToFile();
  #ifdef DEBUG
  Serial.print( "error type:" );
  Serial.println( errorType );
  #endif
  updateDisplay();
}

void clearError()
{
  digitalWrite( STATUS_BAD, LOW);
  digitalWrite( STATUS_OK, HIGH );
  writeErrorToFile();
  systemError = 0;
}


int writeErrorToFile()
{
  //open file
  if( openFileForWrite() != 0 ) return 1;

  //write the record
  getDateTime();
  //    commandString = "type,id,time,date,data\n";
  csvString = "err,";
  csvString.append( POWERPACK_ID );
  csvString.append( ":" );
  csvString.append( useID );
  csvString.append( "," );
  csvString.append( dateString );
  csvString.append( "," );
  csvString.append( systemError ); 

  if( systemError == USB_WRITE_ERROR )
  {
    csvString.append( "," );
    csvString.append( usbString );
  }
  csvString.append( "\n" );  

  commandString = "WRF ";
  commandString.append( csvString.length() );
  if( sendUSBcommand( commandString, 0 ) != 0 ) return 1;

  sendUSBdata( csvString );

  //close the file
  if( closeFileAfterWrite() != 0 ) return 1;
  return 0; 
}

