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

  //setLogNumber(12345);
  setupLogNames();

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
  myFile.print(now.month(), DEC);
  myFile.print("/");
  myFile.print(now.day(), DEC);
  myFile.print(" ");
  myFile.print(now.hour(), DEC);
  myFile.print(":");
  myFile.print(now.minute(), DEC);
  myFile.print(":");
  myFile.print(now.second(), DEC);
  myFile.print(",");
}
void writeError(String errString)
{
  if (!myFile.open(errorFile, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.println( "err opening file");
      return;
  }
  Serial.print( "write err to SD:");
  // if the file opened okay, write to it:
  writeDate();

  myFile.print( "," );
  myFile.println( errString );

  // close the file:
  myFile.close();
  Serial.println("ok");
}

void writeData()
{
  if (!myFile.open(dataFile, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.println( "err opening file");
      return;
  }

  Serial.print("writing SD:");

  // if the file opened okay, write to it:
  myFile.print( getUnixSecs() );
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
  Serial.println("ok");
}

void readFile(const char * filename)
{
  // re-open the file for reading:
  if (!myFile.open(filename, O_READ)) {
      Serial.println( "err opening file");
      return;
  }

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) > 0) Serial.write(data);
  // close the file:
  myFile.close();
}
#else

void writeData(){}
void writeError(String errString){}
#endif
