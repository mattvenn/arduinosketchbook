// Ported to SdFat from the native Arduino SD library example by Bill Greiman
// On the Ethernet Shield, CS is pin 4. SdFat handles setting SS
const int chipSelect = 10;
#include <SdFat.h>
SdFat sd;
SdFile myFile;

void setupSD()
{
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  Serial.println( "SD init");
}
void writeError(String errString)
{
  if (!myFile.open(ERRORFILE, O_RDWR | O_CREAT | O_AT_END)) {
      Serial.println( "err opening file");
      return;
  }
  Serial.print( "write err to SD:");
  // if the file opened okay, write to it:
  myFile.print( getUnixSecs() );
  myFile.print( "," );
  myFile.println( errString );

  // close the file:
  myFile.close();
  Serial.println("ok");
}

void writeData()
{
  if (!myFile.open(DATAFILE, O_RDWR | O_CREAT | O_AT_END)) {
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

