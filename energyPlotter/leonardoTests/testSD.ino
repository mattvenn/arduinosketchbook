/*

the use of cli and sei in this file are to protect the sd read/write operations from being interrupted by the radio. If that happens then the radio can pollute the spi bus

*/

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
#define FILENAME "test1.txt"
#define CFILENAME "commands.txt"
void initSD()
{
  //moving the prints outside the interrupts made a difference. Check on what John was saying.
  //worked for a bit then broke, removing cli and sei made it work again ;( 
  Serial.println("sd init" );
 // delay(500);
  //cli();

   if (!sd.begin(SD_SEL, SPI_SIXTEENTH_SPEED)) 
     {
   //    sei();
       Serial.println( "sd init failed" );
       return;
     }

  
  
  //delay(100);

  //sei();
  Serial.println("done");
}

void storeCommand()
{
  cli();
  //truncate to beginning
  if(storedCommands==0)
  {
  if (!myFile.open(CFILENAME, O_RDWR | O_CREAT | O_TRUNC)) {
    Serial.println( "open for write failed" );
    sei();
    return; //sd.errorHalt("opening test.txt for write failed");
  }
  }
  else
  {
    if (!myFile.open(CFILENAME, O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println( "open for write failed" );
    sei();
    return; //sd.errorHalt("opening test.txt for write failed");
    }
  }
  myFile.write(&payload,sizeof(Payload));
  myFile.close();
  sei();
  cleanSPIBus();
  Serial.println("done");
}

void printStoredCommands(boolean execute)
{
  cli();
  // re-open the file for reading:
  if (!myFile.open(CFILENAME, O_READ)) {
    Serial.println( "open for read failed" );
    sei();
    return;
  }

  // read from the file until there's nothing else in it:

  Payload tmpData;
  unsigned long int lines=0;
  while ((myFile.read(&tmpData,sizeof(tmpData))) > 0) 
  {
    lines ++;
    printPayload(&tmpData);
    //execute if necessary - will this cause problems with interrupts as they are turned off? YES!
    if(execute)
    {
      Serial.println("run.."); 
      //interrupts needed for stuff to run right (delays etc)
      sei();
      runCommand(&tmpData);
      cli();
    }
  }
  payload.arg1 = lines & 0xFFFF;
  payload.arg2 = lines >> 16;
   myFile.close();
  sei();
  cleanSPIBus();

  Serial.print( "read commands: ") ; Serial.println( lines );
  
}
void writeSD(int number)
{

  cli();
    //  delay(100);
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
 
  // open the file for write at end like the Native SD library
  if (!myFile.open(FILENAME, O_RDWR | O_CREAT | O_AT_END)) {
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
    //delay(100);
  sei();
  cleanSPIBus();

}
//puts number of lines read int arg1 and arg2
void readSD()
{
  cli();
    //  delay(100);
   // if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) sd.initErrorHalt();

  // re-open the file for reading:
  if (!myFile.open(FILENAME, O_READ)) {
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
   // delay(100);
  sei();
  cleanSPIBus();
}
#endif
