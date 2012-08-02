/*
void test_SD()
{
  digitalWrite(SD_SEL,LOW);
  digitalWrite(MISO,HIGH);
  delay(200);
    digitalWrite(MISO,LOW);
    delay(200);
  
}
*/
/*
 
 created   Nov 2010
 by David A. Mellis
 updated 2 Dec 2010
 by Tom Igoe
 modified by Bill Greiman 11 Apr 2011
 This example code is in the public domain.
 	 
 */
#ifdef useSD
#include <SdFat.h>
SdFat sd;
SdFile myFile;

void initSD()
{
  //moving the prints outside the interrupts made a difference. Check on what John was saying.
  //worked for a bit then broke, removing cli and sei made it work again ;( 
  Serial.println("sd init" );
  delay(500);
  //cli();

   if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
     {
       sei();
       Serial.println( "sd init failed" );
       return;
     }

  
  
  delay(100);

  //sei();
  Serial.println("done");
}

void writeSD(int number)
{

  cli();
      delay(100);
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
 
  // open the file for write at end like the Native SD library
  if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println( "open for write failed" );
    sei();
    return; //sd.errorHalt("opening test.txt for write failed");
  }
  // if the file opened okay, write to it:
  Serial.print("Writing to test.txt...");
  myFile.println(number);

  // close the file:
  myFile.close();
  Serial.println("done.");
    delay(100);
  sei();

}
//puts number of lines read int arg1 and arg2
void readSD()
{
  cli();
      delay(100);
    if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  // re-open the file for reading:
  if (!myFile.open("test.txt", O_READ)) {
    Serial.println( "open for read failed" );
    sei();
    return;
    
    //sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println("test.txt:");

  // read from the file until there's nothing else in it:
  int data;
  unsigned long int lines=0;
  while ((data = myFile.read()) > 0) 
  {
    lines ++;
    //Serial.write(data);
  }
  payload.arg1 = lines & 0xFFFF;
  payload.arg2 = lines >> 16;
  Serial.print( "lines: ") ; Serial.println( lines );
  // close the file:
  myFile.close();
    delay(100);
  sei();
}
#endif
