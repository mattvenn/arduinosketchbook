// Ported to SdFat from the native Arduino SD library example by Bill Greiman
// On the Ethernet Shield, CS is pin 4. SdFat handles setting SS
#ifdef useSD
#include <SdFat.h>
const int chipSelect = 10;
SdFat sd;
SdFile myFile;

#define EEADDRlogNumberLow 0
#define EEADDRlogNumberHigh 1

//String dataFile;
char dataFile[] = "dat00000.csv";
char errorFile[]= "err00000.csv";

void setupSD()
{
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  Serial.println( "SD init");

  setupLogNames();
  writeHeader();

}

void setupLogNames()
{
  unsigned int logNumber = readLogNumber();
  logNumber ++;
  writeLogNumber(logNumber);

  dataFile[3] = (logNumber/10000) %10 + '0';
  dataFile[4] = (logNumber/1000) %10 + '0';
  dataFile[5] = (logNumber/100) %10 + '0'; 
  dataFile[6] = (logNumber/10) %10 + '0'; 
  dataFile[7] = logNumber %10 + '0'; 

  //copy the number
  for( int i = 3; i <= 7; i ++ )
    errorFile[i] = dataFile[i];

  Serial.println( errorFile );
  Serial.println( dataFile );
}

unsigned int readLogNumber()
{
  byte logNumberLow = EEPROM.read(EEADDRlogNumberLow);
  byte logNumberHigh = EEPROM.read(EEADDRlogNumberHigh);
  unsigned int logNumber = (logNumberHigh<<8)+logNumberLow;
  return logNumber;
}

void writeLogNumber (unsigned int logNumber) 
{
  byte logNumberLow = logNumber;
  byte logNumberHigh = logNumber>>8;
  EEPROM.write(EEADDRlogNumberLow,logNumberLow);
  EEPROM.write(EEADDRlogNumberHigh,logNumberHigh);
}
void writeDate()
{
  DateTime now = RTC.now();
  myFile.print(now.year(), DEC);
  myFile.print("/");
  printFileDigits( now.month() );
  myFile.print("/");
  printFileDigits(now.day());
  myFile.print(" ");
  printFileDigits(now.hour());
  myFile.print(":");
  printFileDigits(now.minute());
  myFile.print(":");
  printFileDigits(now.second());
}

void printFileDigits(byte digits){
  // utility function for digital clock display: prints colon and leading 0
  if(digits < 10)
    myFile.print('0');
  myFile.print(digits,DEC);   
}
void writeError(String errString)
{
  if (!myFile.open(errorFile, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.print(F("err opening: "));
      Serial.println( errorFile );
      return;
  }
  Serial.println(F("writing err to SD"));
  // if the file opened okay, write to it:
  writeDate();

  myFile.print( "," );
  myFile.println( errString );

  // close the file:
  myFile.close();
  Serial.println(F("OK"));
}

void writeHeader()
{
  if (!myFile.open(dataFile, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.print(F("err opening: "));
      Serial.println( dataFile );
      return;
  }
  //Serial.println( "writing header" );
  Serial.println(F("writing header"));
  myFile.println( F("date,status,ambient temp,stack V,stack I,stack temp,id,uptime") );
  myFile.close();
}

void writeData()
{
  if (!myFile.open(dataFile, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.print(F("err opening: "));
      Serial.println( dataFile );
      return;
  }

  Serial.println(F("writing data"));

  // if the file opened okay, write to it:
  //myFile.print( getUnixSecs() );
  writeDate();
  myFile.print( "," );
  myFile.print( message.fuelcellStatus );
  myFile.print( "," );
  myFile.print( message.fuelcellAmbientT );
  myFile.print( "," );
  myFile.print( message.fuelcellStackV );
  myFile.print( "," );
  myFile.print( message.fuelcellStackI );
  myFile.print( "," );
  myFile.print( message.fuelcellStackT );
  myFile.print( "," );
  myFile.print( message.id, DEC );
  myFile.print( "," );
  myFile.print( message.uptime );

  myFile.println("");

  // close the file:
  myFile.close();
  Serial.println(F("OK"));
}

#else
void setupSD(){};
void writeData(){}
void writeError(String errString){}
#endif
