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
#define POWERPACK_ID "pp5"
#define HWV1_1
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






