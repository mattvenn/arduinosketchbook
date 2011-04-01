// write the CSV headers
int writeCSVHeaders()
{
#ifdef DEBUG_1
  Serial.println( "write CSV head" );
#endif
    //open file
    if( openFileForWrite() != 0 ) return 1;

    commandString = "type,id,time,date,data\n";
    int noOfChars = commandString.length();
    commandString = "WRF ";
    commandString.append( noOfChars );
    if( sendUSBcommand( commandString, 0 ) != 0 ) return 1;

    commandString = "type,id,time,date,data\n";
    sendUSBdata( commandString );

    //close the file
    if( closeFileAfterWrite() != 0 ) return 1;
    return 0;

}


/*
writes the current data to the usb stick.
returns 1 if there is a problem. This will be because of possible failures of sendUSBcommand()
*/
int writeToFile( )
{ 
  
  //open file
  if( openFileForWrite() != 0 ) return 1;
  
  //write the record
  getDateTime();
  csvString = "log,";
  csvString.append( POWERPACK_ID );
  csvString.append( ":" );
  csvString.append( useID );
  csvString.append( "," );
  csvString.append( dateString );
  csvString.append( "," );
  csvString.append(fuelcell.stackV );
  csvString.append( "," );
  csvString.append( (int)fuelcell.current );
  csvString.append( "," );
    csvString.append( fuelcell.stackT );
  csvString.append( "," );
  csvString.append( fuelcell.status );
  csvString.append( "," );
  csvString.append( (int)battLevel );
  csvString.append( "\n" );
  // we may run out of csv string's capacity at some point.
  #ifdef DEBUG_1
  Serial.print( "csv capacity: " );
  Serial.println( csvString.capacity() - csvString.length() );
#endif
  if( csvString.capacity() - csvString.length() <= 0 )
  {
    error( DATA_TOO_LARGE );
    return 1;
  }
  
  
  commandString = "WRF ";
  commandString.append( csvString.length() );
  if( sendUSBcommand( commandString, 0 ) != 0 ) return 1;
  sendUSBdata( csvString );

  //close the file
  if( closeFileAfterWrite() != 0 ) return 1;
  
  
  return 0;


}

/*
closes a file after writing
returns 1 on failure, 0 on success
*/
int closeFileAfterWrite()
{
 #ifdef DEBUG_2
  Serial.print( "closefile: " );
  Serial.println( fileNameString );
#endif
  commandString = "CLF ";
  commandString.append( fileNameString );
  if( sendUSBcommand( commandString, 1 ) != 0 ) return 1;
  return 0;
}

/*
opens a file ready for writing
returns 1 on failure, 0 on success
*/
int openFileForWrite()
{
  #ifdef DEBUG_2
  Serial.print( "open file: " );
  Serial.println( fileNameString );
#endif
  //open and write file
  commandString = "OPW ";
  commandString.append( fileNameString ); 
  commandString.append( dateAsHexString );
  if( sendUSBcommand( commandString, 1 ) != 0 ) return 1;
  return 0;
}

/*
set the IO lines in initial state,
configure the usb's virtual serial port,
wait for the USB disk to come on line,
send the command to talk in the easier (for us humans) to read/write ASCII mode
*/

void setupUSBComms()
{
   //tell the USB that we're not ready to read
  digitalWrite( USB_READY_TO_READ, HIGH ); //not ready

  // set the data rate for the USB's virtual serial port
  usb.begin(9600);
  usb.flush();
  usbString = "";
  // reset the USB device
  digitalWrite( STATUS_BAD, HIGH );
  digitalWrite( USB_RESET, LOW );
  delay( 100 );
  digitalWrite( USB_RESET, HIGH );
  digitalWrite( STATUS_BAD, LOW);
  //wait for device to come online
 
}
int setupUSBDisk()
{
  // turn on ascii mode commands
  commandString = "IPA";
  if( sendUSBcommand( commandString, 1 ) )
 {
   // XXX this is returning 1 atm
   // maybe it doesn't return the D:/?
//   return 1;
 }

  //get new filename
  useID = EEPROMReadInt( useEepromAddress );
//  useID = 1;
  EEPROMWriteInt( useEepromAddress, useID + 1 );
  fileNameString = POWERPACK_ID;
  fileNameString.append( "-" );
  fileNameString.append( useID );
  fileNameString.append( ".CSV" );

  //write CSV headers if it's a new file
  if( writeCSVHeaders() == 1 )
  {
    error( USB_WRITE_ERROR );
    return 1;
  }
  usbReadyToLog = 1;
  return 0;

}

/*
waits indefinately for USB disk to be ready.

we wait for it to print out D:\>
which means there is a disk present
*/
void waitForUSBDisk()
{ 
#ifdef DEBUG_1
  Serial.print( "Wait on USB" );
#endif

  //keep polling till we get the disk ready prompt
  while( ! usbString.contains( "D:\\>" ) )
  {
    //this will hang till we get something
    digitalWrite( STATUS_BAD , HIGH );
    getLineFromUSB();
    delay( 100 );
    #ifdef DEBUG_1
    Serial.print( ".");
    #endif
        digitalWrite( STATUS_BAD , LOW);
  }
#ifdef DEBUG_1
  Serial.println("");
  Serial.print("Disk ready:");
  Serial.println( usbString );
  
#endif
  usb.flush();
}


boolean usbDiskAvailable()
{
  usb.print( 13, BYTE );
  getLineFromUSB();
  if( usbString.contains( "D:\\>" ) )
    return true;
  return false;
}
  
/*
send a command to the usb device, returns 0 on success, 1 on failure

can omit checking for new prompt by setting second argument to 0. This is useful for the data writing, because first you send a command,
then write the data. You only get a reply after you've finished writing the data

if it fails, then this error should be flowed up by functions that use it
there are 2 ways it can fail:
1/- the usb device isn't ready to write to (hardware issue)
2/- we write the command but don't get the D:/ that indicates a success (eg, disk full, bad filename, buffer overflow etc)
*/
int sendUSBcommand( char * command, int check )
{
#ifdef DEBUG_2 
  Serial.print( "send to usb: " );
  Serial.println( command );
#endif
  if( isUSBReadyToReceive() == 1 )
  {
    error( USB_NOT_READY );
    return 1;
  }
  usb.print( command );
  usb.print( 13, BYTE );
  
  //if we're checking that we get a good output...
  if( check == 0 ) return 0;
  getLineFromUSB();
#ifdef DEBUG_2 
  Serial.println("got from usb: ");
  Serial.println(usbString);    
#endif
  
  if( usbString.contains( "D:\\>" ) )
  {
    return 0;
  }
  else if( usbString.contains( "Device Removed" ) )
  {
    error( USB_DEVICE_REMOVED );
    return 1;
  }
  else
  {
    error( USB_COMMAND_FAIL );
    return 1;
  }
}

void sendUSBdata( char * data )
{
  if( isUSBReadyToReceive() == 1 ) error( USB_NOT_READY );
#ifdef DEBUG_2
  Serial.println( data );
#endif
  usb.print( data );
}
/*
wait till we get a single line from the USB device.
We're looking for a carriage return to denote new lines
the new line is put into a global string: usbstring
*/
void getLineFromUSB()
{
  usbString = "";
  //usb.flush();
  boolean newLine = false;
  //tell usb that we're ready to read
  digitalWrite( USB_READY_TO_READ, LOW );
  delay( 50 );
  int chars = usb.available();
  #ifdef DEBUG_2
  Serial.print( "chars in buffer: " );
  Serial.println( chars ) ;
  
  #endif
  int tries = 0;

  //lcd.clear();
  while( ( ! newLine ) and ( tries ++ < 100 ) )
  {
    if( usb.available() )
    {    
      char usbChar = (char)usb.read();
   //   lcd.print( usbChar );
      if (usbString.length() < maxLengthUSBString - 1)
      {
        usbString.append(usbChar);
      }
      else
      {
        //flag this surely!
        usbString = "";
        usbString.append( usbChar );
      }  
      //if ends with carriage return then return
      if( usbChar == 0xD and usb.available() == 0 )
      {
        newLine = true;
      } 
    }
   delay( 10 ); 
  }
  //lcd.print( usbString );
  #ifdef DEBUG_2
  Serial.print( "usb said: " );
  Serial.println( usbString );
  #endif
  //tell usb we're not ready to read
  digitalWrite( USB_READY_TO_READ, HIGH );
}


/*
low level check to see if the usb device is ready for us to send a command
we use this before we send any command. We wait for a max of 100ms and then give up - which should cause
an error to flow up

returns 0 if ready
*/
int isUSBReadyToReceive()
{
  for( int i = 0; i < 100 ; i ++ )
  {
    if( digitalRead( USB_READY_TO_WRITE ) == LOW )
      return 0 ;
    //wait
    delay( 10 );
  }
  //usb device isn't ready to receive data
  return 1;
}
