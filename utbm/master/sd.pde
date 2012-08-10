// Ported to SdFat from the native Arduino SD library example by Bill Greiman
// On the Ethernet Shield, CS is pin 4. SdFat handles setting SS
const int chipSelect = 10;
#include <SdFat.h>
SdFat sd;
SdFile myFile;
#define FILENAME "data2.txt"

void setupSD()
{
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  Serial.println( "SD initialized");
}

void writeData()
{
  // open the file for write at end like the Native SD library
  if (!myFile.open(FILENAME, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening file for write failed");
  }
  // if the file opened okay, write to it:
  Serial.print("writing to SD...");
  myFile.print( message.voltage );
  myFile.print( "," );
  myFile.print( message.current );
  myFile.print( "," );
  myFile.print( message.status, DEC );
  myFile.print( "," );
  myFile.print( message.id, DEC );
  myFile.print( "," );
  myFile.print( message.uptime );
  myFile.println("");

  // close the file:
  myFile.close();
  Serial.println("done.");
}

void readData()
{
  // re-open the file for reading:
  if (!myFile.open(FILENAME, O_READ)) {
    sd.errorHalt("opening file for read failed");
  }

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) > 0) Serial.write(data);
  // close the file:
  myFile.close();
}

