#include <MsTimer2.h>

#include <WString.h>
#include <NewSoftSerial.h>
#include <LiquidCrystal.h>

//error defs
#define USB_NOT_READY 1
#define USB_COMMAND_FAIL 2
#define USB_WRITE_ERROR 3

//pin defs
#define USB_RESET 5
#define USB_READY_TO_WRITE 4
#define USB_TX 2
#define USB_RX 3
#define LED_OK 13
#define ANALOG_IN 5

#define maxLength 30
String usbString = String(maxLength);
String commandString = String(maxLength);

//globals
int analogInput;
int recordNumber;
int systemError;

NewSoftSerial usb(USB_TX, USB_RX);

LiquidCrystal lcd(6,7,8,9,10,11); //rs, enable, d4->d7
void setup()  
{ 
  //globals
  recordNumber = 0;
  systemError = 0;
  
  //lcd setup
  lcd.begin(16, 2);
  lcd.print("Arcola Energy");
  
  //serial setup
  Serial.begin(9600);
  
  //pin setup
  pinMode( USB_READY_TO_WRITE, INPUT );
  pinMode( USB_RESET, OUTPUT );
  pinMode( LED_OK, OUTPUT );
  digitalWrite( LED_OK, LOW );
  digitalWrite( USB_RESET, HIGH );
  
  // set the data rate for the NewSoftSerial port
  usb.begin(9600);
  // reset the usb device
  digitalWrite( USB_RESET, LOW );
  delay( 100 );
  digitalWrite( USB_RESET, HIGH );
  
  waitForUSBDisk();
  
  // turn on ascii mode commands
  if( sendUSBcommand( "IPA", 1 ) ) error( USB_COMMAND_FAIL );
  // delete existing log file
  sendUSBcommand( "DLF log.txt", 0 );
  
  delay( 500 );
  MsTimer2::set( 100, updateDisplay);
  MsTimer2::start();
}

void waitForUSBDisk()
{ 
  Serial.println( "Waiting for USB" );
  lcd.setCursor( 0, 1 );
  lcd.print( "waiting for USB" );
  //keep polling till we get the disk ready prompt
  while( ! usbString.contains( "D:\\>" ) )
  {
    getLineFromUSB();
    delay( 100 );
  }
  Serial.println("USB Disk ready");
  lcd.setCursor( 0,1 );
  lcd.print( "USB disk ready" );
}

void updateDisplay()
{
  lcd.clear();
  if( systemError )
  {
   lcd.print( "error" ); 
  }
  else
  {
  lcd.print( "logging..." );
  lcd.setCursor( 0, 1 );
  
  analogInput = analogRead( ANALOG_IN );

  lcd.print( recordNumber );
  lcd.print( " : " );
  lcd.print( analogInput );
  }
}

int isUSBReady()
{
  for( int i = 0; i < 10 ; i ++ )
  {
    if( digitalRead( USB_READY_TO_WRITE ) == LOW )
      return 0 ;
    //wait
    delay( 10 );
  }
  //usb device isn't ready to receive data
  return 1;
}

//returns 0 on success, 1 on failure
int sendUSBcommand( char * command, int check )
{
  Serial.print( "sending to usb: " );
  Serial.println( command );
  if( isUSBReady() == 1 ) error( USB_NOT_READY );
  usb.print( command );
  usb.print( 13, BYTE );
  if( check == 0 ) return 0;
  getLineFromUSB();
  Serial.println(usbString);    
  return ! usbString.contains( "D:\\>" );
}

//poll usb.available
//put data into usbString
void getLineFromUSB()
{
  usbString = "";
  //usb.flush();
  char usbReady = 0;
  while( usbReady == 0 )
  {
    if( usb.available() )
    {    
      char usbChar = (char)usb.read();
      if (usbString.length() < maxLength)
      {
        usbString.append(usbChar);
      }
      else
      {
        usbString = usbChar;
      }  
      //if ends with carriage return then return
      if( usbChar == 0xD )
      {
        usbReady = 1;
      } 
    } 
  }
}

void error( char errorType )
{
  Serial.print( "error number: " );
  Serial.print( errorType );
  Serial.print("\n");
  systemError = errorType;
}
void clearError()
{
  systemError = 0;
}
int writeToFile( int data )
{ 
  Serial.println( "open/create file" );
  //open and write file
  if( sendUSBcommand( "OPW log.txt", 1 ) != 0 ) return 1;

  //write the record
  int noOfChars=1;
  int x=data;                       // need to copy valToWrite as getting no of characters will consume it
  while (x>= 10){                     // counts the characters in the number
    noOfChars++;                      // thanks to D Mellis for this bit
    x/=10;     
  }
  noOfChars +=1;                      //add 1 to the num for carriage return 
    
  commandString = "WRF ";
  commandString.append( noOfChars );
  if( sendUSBcommand( commandString, 0 ) != 0 ) return 1;

  //convert data to string
  itoa( data, commandString, 10 );
  if( sendUSBcommand( commandString, 1 ) != 0 ) return 1;

  //close the file
  Serial.println( "closing file" );
  if( sendUSBcommand( "CLF log.txt", 1 ) != 0 ) return 1;
  Serial.println( "finished write" ); 
  return 0;
}

void loop()                     // run over and over again
{
  digitalWrite( LED_OK, HIGH );
  delay( 1000 );
  digitalWrite( LED_OK, LOW );
  delay( 1000 );
  
  recordNumber ++;
  MsTimer2::stop();
  if( writeToFile( analogInput ) != 0 )
  {
    error( USB_WRITE_ERROR );
    waitForUSBDisk();
    clearError();
  }
  MsTimer2::start();
  
}  


