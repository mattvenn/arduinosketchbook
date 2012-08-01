void test_SD()
{
  digitalWrite(SD_SEL,LOW);
  digitalWrite(MISO,HIGH);
  delay(200);
    digitalWrite(MISO,LOW);
    delay(200);
  
}

/*
 
 created   Nov 2010
 by David A. Mellis
 updated 2 Dec 2010
 by Tom Igoe
 modified by Bill Greiman 11 Apr 2011
 This example code is in the public domain.
 	 
 */
#include <SdFat.h>
SdFat sd;
SdFile myFile;

void writeSD(int number)
{
  cli();
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  // open the file for write at end like the Native SD library
  if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  // if the file opened okay, write to it:
  Serial.print("Writing to test.txt...");
  myFile.println(number);

  // close the file:
  myFile.close();
  Serial.println("done.");
  sei();

}

void readSD()
{
  cli();
    if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  // re-open the file for reading:
  if (!myFile.open("test.txt", O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println("test.txt:");

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) > 0) 
  {
    //Serial.write(data);
  }
  // close the file:
  myFile.close();
  sei();
}

