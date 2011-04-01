/*
arcola energy monitor 
 author Matthew Venn (matt@mattvenn.net)
 powerpacks.arcolaenergy.com
 2009, 2010
 
 todo:
 1/- done: define what we do with writing/creating new log files.
 2/- sometimes removing disk can lead to corruption. This stops the logger from working properly. how to fix?
 3/- done: filename is truncated to 8.3. ours are currently longer
 4/- done: get logging more regular
 5/- done: write errors to separate error analyis file for debug
 6/- done:fix usb write issue
 7/- done: refactor code
 8/- done: writeCSVheaders is often writing the headers in the middle of the file
 9/- done: usbDiskReady routine is broken. returns ok with no disk in!
 10/- done: when usb stick is removed, we should increment usage id
 11/- done: implement error to disk writing
 12/- done: show fuel cell errors on LCD
 13/- done: detect when fuel cell not present and wipe struct
 14/- done: datestamp files
 15/- check too_much_data error works!
 16/- done: be able to monitor battery charge level
 17/- fix full disk errors
 18/- try to unmount usb on powerdown 
 19/- auto power off to save battery?
 20/- usb still unreliable!!
 21/- don't hang on usb problem
 */

//#include <MsTimer2.h>
#include <WProgram.h>
#include <Wire.h>
#include <DS1307.h> // written by  mattt on the Arduino forum and modified by D. Sjunnesson
#include <MemoryFree.h>
#include <EEPROM.h>
#include <WString.h>
#include <NewSoftSerial.h>
#include <LiquidCrystal.h>

//id def
#define POWERPACK_ID "pp1"
#define HWV1_0
#define SWV 1.1

//error defs
#define USB_NOT_READY 1
#define USB_COMMAND_FAIL 2
#define USB_WRITE_ERROR 3
#define FUELCELL_NOT_READY 4
#define USB_DEVICE_REMOVED 5
#define DATA_TOO_LARGE 6
#define LOW_BATTERY 7
#define USB_DEVICE_FAILED 8

#define abortUSB 100 //times to retry USB

//status defs
#define SETUP_USB 1
#define WAIT_ON_FUELCELL 2
#define LOGGING 3
#define SHUTDOWN 4
#define RUNNING 5
#define USB_READY 6

#ifdef HWV1_1
//level defs
#define BATT_ADC_RATIO 72
#define FUEL_ADC_RATIO 10
#define BATT_OK_LEVEL 10 * BATT_ADC_RATIO //10 volts with board v1
#define MAINS_OK_LEVEL 300 //29/5/10 realised that dc/dc won't turn on reliably with psu V > 12v!

//pin defs
#define STATUS_OK 13 //0 26/05/10
#define STATUS_BAD 11 //1 26/05/10
#define USB_RESET 5 // RS#
#define USB_READY_TO_WRITE 4 //pin6 //AD2 RTS - output from the USB, when low we can send
#define USB_READY_TO_READ 1 //pin18 AD3 CTS - input to USB, when we put it low it can send 26/05/10
#define USB_TX 12  //pin25 AD0 TX of usb serial device (we listen to this)
#define USB_RX 3  //pin5 AD1 RX of usb serial device (we talk to this)
#define FUEL_CELL_TX 0 //the TX of the fuel cell (we listen to this) 26/05/10
#define BATT_RELAY 16 //analog pin 2 26/05/10
#define OFF_SWITCH 2 //int 0

#include "WProgram.h"
void setup();
void shutdown();
void switchOffInt();
void loop();
void updateStatus( int newStatus);
void updateDisplay();
void lcdPrintDouble( double val, byte precision);
void EEPROMWriteInt(int p_address, int p_value);
unsigned int EEPROMReadInt(int p_address);
void displayError();
void error( int errorType );
void clearError();
int writeErrorToFile();
int updateFuelCell();
void clearFuelCellData();
void setupFuelCellSerial();
int getSeconds();
void getDateTime();
int writeCSVHeaders();
int writeToFile( );
int closeFileAfterWrite();
int openFileForWrite();
void setupUSBComms();
int setupUSBDisk();
void waitForUSBDisk();
boolean usbDiskAvailable();
int sendUSBcommand( char * command, int check );
void sendUSBdata( char * data );
void getLineFromUSB();
int isUSBReadyToReceive();
LiquidCrystal lcd(6,20,7,8,9,10,17); //rs, rw (not used), enable, d4->d7 26/05/10
#endif

#ifdef HWV1_0
//level defs
#define BATT_ADC_RATIO 71
#define FUEL_ADC_RATIO 10
#define BATT_OK_LEVEL 10 * BATT_ADC_RATIO //10 volts with board v1
#define MAINS_OK_LEVEL 500

//pin defs
#define STATUS_OK 0
#define STATUS_BAD 1
#define USB_RESET 5 // RS#
#define USB_READY_TO_WRITE 4 //pin6 //AD2 RTS - output from the USB, when low we can send
#define USB_READY_TO_READ 12 //pin18 AD3 CTS - input to USB, when we put it low it can send
#define USB_TX 16  //pin25 AD0 TX of usb serial device (we listen to this)
#define USB_RX 3  //pin5 AD1 RX of usb serial device (we talk to this)

#define FUEL_CELL_TX 13 //the TX of the fuel cell (we listen to this) 
#define BATT_RELAY 17 //analog pin 3
#define OFF_SWITCH 2 //int 0

LiquidCrystal lcd(6,20,7,8,9,10,11); //rs, rw (not used), enable, d4->d7
#endif


// define this for verbosity on the serial port
//#define DEBUG_1
// and add this for even more verbosity
//#define DEBUG_2

//constants
#define BATTERY 1
#define FUELCELL 2
#define MAINS 3

#define useEepromAddress 1
#define maxLengthUSBString 50
#define maxLengthCommandString 40
#define maxLengthDataString 80

String usbString = String(maxLengthUSBString);
String csvString = String( maxLengthDataString );
String commandString = String(maxLengthCommandString);
String dateString = String(20);
String dateAsHexString = String(13);

//01:01:01,01/01/2009 = 19 chars
String fileNameString = String(8 + 1 + 3);
String errorFileNameString = String(8 + 1 + 3);
//25-12-2009.CSV = 14 chars
//char timeString[8];

#define LOG_INTERVAL 30 //seconds

//type defs
typedef struct {
  unsigned char status;
  unsigned char stackT;
  unsigned char stackV;
  unsigned char stackI;
  unsigned char stackT2;
  unsigned char stackT3;
  unsigned int rawcurrent;
  float current;
  boolean updated;

} 
fuelCellData;


//globals
int analogInput;
int recordNumber;
int systemError;
int systemStatus;
int oldLogSeconds = 0;
int oldLogMinutes = 0;
int useID;
fuelCellData fuelcell;
char fuelcellStatus;
float battLevel;
float mainsLevel;
char powerFrom = BATTERY;
volatile boolean switchOff = false;
volatile int ints = 0;
int lowBatteryTimeOut = 0;
boolean usbReadyToLog = false;

//software serial - first arg is the TX of the device (that we listen to)
NewSoftSerial usb(USB_TX, USB_RX);
NewSoftSerial fuelcellSerial(FUEL_CELL_TX, 0); //tx, rx

void switchOffInt();
void setupFuelCellSerial();
void getDateTime();
void updateStatus( int newStatus);
int getSeconds();
void error( int errorType );
void clearError();
int writeToFile( );
int updateFuelCell();
void shutdown();
void switchOffInt();
void updateStatus( int newStatus);
void updateDisplay();
void EEPROMWriteInt(int p_address, int p_value);
void displayError();
void error( int errorType );
void clearError();
void clearFuelCellData();
void setupFuelCellSerial();
void getDateTime();
void getDate();
void waitForUSBDisk();
void sendUSBdata( char * data );
void getLineFromUSB();
void lcdPrintDouble( double val, byte precision);
int writeErrorToFile();
int updateFuelCell();
int getSeconds();
int writeCSVHeaders();
int writeToFile( );
int closeFileAfterWrite();
int openFileForWrite();
int setupUSBDisk();
int sendUSBcommand( char * command, int check );
int isUSBReadyToReceive();
void setupUSBComms();
boolean usbDiskAvailable();

void setup()  
{ 
  //globals
  recordNumber = 0;
  fuelcellStatus = LOW;

  //pin I/O
  pinMode( STATUS_OK, OUTPUT );
  pinMode( STATUS_BAD, OUTPUT );

  pinMode( USB_READY_TO_WRITE, INPUT ); //if this is high then we can write
  pinMode( USB_READY_TO_READ, OUTPUT ); //write this high when we are ready to recieve from usb
  digitalWrite( USB_READY_TO_READ, HIGH ); //not ready to read the USB
  pinMode( USB_RESET, OUTPUT );
  digitalWrite( USB_RESET, HIGH );
  pinMode( BATT_RELAY, OUTPUT );
  pinMode( OFF_SWITCH, INPUT );

  digitalWrite(OFF_SWITCH, HIGH);  //turn on pull up resistor
  digitalWrite( STATUS_OK, HIGH );
  digitalWrite( STATUS_BAD, LOW );
  //turn on battery,
  digitalWrite( BATT_RELAY, HIGH );

  //serial setup -> debugging to host pc
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println( "setup" );
#endif
  //lcd setup
  lcd.begin(20, 4);
  lcd.clear();
  lcd.print("http://powerpacks." );
  lcd.setCursor( 0,1 );
  lcd.print("ArcolaEnergy.com");
  lcd.setCursor( 0,3 );
  lcd.print("ID: ");
  lcd.print( POWERPACK_ID );
  lcd.print(" SW: " );
  lcd.print( SWV );
  delay( 1500 );

  //off switch interrupt
  attachInterrupt(0, switchOffInt, LOW);


  lcd.clear();
  //lcd.print( "turn on fuel cell.." );

  //setup fuel cell serial
  setupFuelCellSerial();

  //update date and time strings so that new file is written with correct date stamp
  getDateTime();
#ifdef DEBUG
  Serial.print( "date is: " );
  Serial.println( dateString );  
#endif

  //setup port and reset the device
  setupUSBComms();

}

void shutdown()
{
  detachInterrupt( 0 );
  digitalWrite( STATUS_BAD, HIGH );
  lcd.clear();
  lcd.setCursor( 0, 0 );
  if( powerFrom == BATTERY )
  {
    lcd.print( "switching off.." );    
    while( digitalRead( OFF_SWITCH ) == LOW )
    { }
    digitalWrite( BATT_RELAY, LOW );
    while( 1 ) {
    }
  }
  else
  {
    lcd.print( "switch off mains");
    lcd.setCursor( 0, 1 );
    lcd.print( "and fuel cell first" );

    while( digitalRead( OFF_SWITCH ) == LOW )
    {
      // nothing
    }
    digitalWrite( STATUS_BAD, LOW );
    attachInterrupt(0, switchOffInt, LOW);
  }
}

void switchOffInt()
{
  ints ++;
  if( ints > 30 )
  {
    //shut down
    ints = 0;
    shutdown();
  }
}

void loop()
{
  int seconds = getSeconds();

  if( seconds != oldLogSeconds )
  {
    oldLogSeconds = seconds;

    
    //update battery and mains levels
    battLevel = analogRead( 0 );
    mainsLevel = analogRead( 1 );

    if( mainsLevel > MAINS_OK_LEVEL )
    {
      //we're running on mains, so batt is charging
      powerFrom = MAINS;
    }
    else if( fuelcellStatus == HIGH )
    {
      //no mains, but fuel cell OK
      powerFrom = FUELCELL;
    }
    else
    {
      //no power!
      powerFrom = BATTERY;
      if(  battLevel < BATT_OK_LEVEL )
      {
        //battery is too low, so shutdown
        error( LOW_BATTERY );
        if( lowBatteryTimeOut ++ >= 2 )
          shutdown();
      }
      else
      { 
        //battery is OK
        if( systemError == LOW_BATTERY )
        {
          clearError();
          lowBatteryTimeOut = 0;
        }
      }
    }   
    updateStatus( RUNNING );
   
  }

  if( seconds % LOG_INTERVAL == 0 )
  {
    ints = 0; //reset interrupt counter HACK!
    if( usbDiskAvailable() )
    {
      if( usbReadyToLog == true )
      {
        updateStatus( LOGGING );
        recordNumber ++;

        //write the current data to usb disk
        if( writeToFile() != 0 )
        {
     
          //clear the error (also writes the error to the file
          clearError();
        }
      }
      else
      {
        updateStatus( SETUP_USB );
        setupUSBDisk();
      }

    }


    //keep track of free mem
#ifdef DEBUG
    Serial.print("freeMem: ");
    Serial.println( freeMemory() );  
#endif
    //check the usb disk
    //setup usb disk - this will hang till USB is ready

      //update fuel cell details
    updateStatus( WAIT_ON_FUELCELL );
    if( updateFuelCell() != 0 )
    {
      //if it fails, set error condition and wait till we get something
      fuelcellStatus = LOW;
      //      error( FUELCELL_NOT_READY );
    }
    else
    {
      //   clearError(); //problem with this is that it will clear other errors too.
      fuelcellStatus = HIGH;
    }


  }  
}  






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

//http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1218921214/0
//thanks to allsystemsgo
//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}


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

//returns 1 if new data has arrived
int updateFuelCell()
{
  for( int i = 0; i < 20 ; i ++ )
    {
  //if data available, update the fuelcell structure
  if (fuelcellSerial.available() > 8 ) 
  {
    #ifdef DEBUG_1
  Serial.println( "got data from fuelcell" );
  #endif

       fuelcell.status = fuelcellSerial.read();
       fuelcell.stackT = fuelcellSerial.read();
       fuelcell.stackV = fuelcellSerial.read();
       fuelcell.stackI = fuelcellSerial.read();
       fuelcell.stackT2 = fuelcellSerial.read();
       fuelcell.stackT3 = fuelcellSerial.read();
       fuelcell.rawcurrent = fuelcellSerial.read() * 256; //high byte
       fuelcell.rawcurrent += fuelcellSerial.read(); //low byte

       //calculate actual current
       fuelcell.current = fuelcell.rawcurrent * 0.00039;
       fuelcell.updated = HIGH;

       return 0;
  }
  delay( 100 );
    }
    //wipe out the old data
    clearFuelCellData();
  return 1;
}

void clearFuelCellData()
{
       fuelcell.status = 0;
       fuelcell.stackT = 0;
       fuelcell.stackV = 0;
       fuelcell.stackI = 0;
       fuelcell.stackT2 = 0;
       fuelcell.stackT3 = 0;
       fuelcell.rawcurrent = 0;
       fuelcell.current = 0;
       fuelcell.updated = LOW;
}

//setup - very simple as there is no flow control or talking required on our behalf - we just listen
void setupFuelCellSerial()
{
 fuelcellSerial.begin(9600); 
}

int getSeconds()
{
  return RTC.get(DS1307_SEC,true);//read seconds
}

void getDateTime()
{
  dateString = "";
  int hour,minute,sec,year,month,day;
  hour = RTC.get(DS1307_HR,true);  //read the hour and also update all the values by pushing in true
  if( hour < 10 ) dateString.append("0") ;
  dateString.append(hour);
  dateString.append(":");
  minute = RTC.get(DS1307_MIN,false);//read minutes without update (false)
  if( minute < 10 ) dateString.append("0") ;
  dateString.append(minute);
  dateString.append(":");
  sec = RTC.get(DS1307_SEC,false);//read seconds
  if( sec < 10 ) dateString.append("0") ;
  dateString.append(sec);
  dateString.append(",");

  day = RTC.get(DS1307_DATE,false);//read date
  if( day < 10 ) dateString.append("0") ;
  dateString.append(day);
  dateString.append("/");

  month = RTC.get(DS1307_MTH,false);//read date
  if( month < 10 ) dateString.append("0") ;
  dateString.append(month);
  dateString.append("/");

  year = RTC.get(DS1307_YR,false); //read year 
  dateString.append(year); 

/*
formats a string like " 0x36C77319" that is used to date stamp the files on the usb stick
*/
   //vdip1 year 0 == 1980
   year -= 1980;
   //vdip1 sec is 0->30
   sec /= 2;
   unsigned int datecalc=(year<<9)|(month<<5)|(day);
   unsigned int lsdatecalc=(hour<<11)|(minute<<5)|(sec/2);
   char buffer[ 14 ];
   sprintf( buffer, " 0x%X%X", datecalc,lsdatecalc );
   dateAsHexString = buffer;
   #ifdef DEBUG_2
   Serial.print( "hex date string:" );
   Serial.println( dateAsHexString );
   #endif
}

/*
void getDate()
{
  dateString = "";
  dateString.append(RTC.get(DS1307_DATE,true));//read date
  dateString.append("-");
  dateString.append(RTC.get(DS1307_MTH,false));//read month
  dateString.append("-");
  dateString.append(RTC.get(DS1307_YR,false)); //read year 

}
*/
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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

