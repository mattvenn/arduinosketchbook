/*
fetches hymera tty from the 168
*/


#include <SPI.h>
#include "pins_arduino.h"

void setup_hymera(void)
{
  pinMode(mega_168_cs,OUTPUT);
  digitalWrite(mega_168_cs, HIGH); 
 // digitalWrite(SS, HIGH);  // ensure SS stays high for now

  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin ();

  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  
}  // end of setup

byte transferAndWait (const byte what)
{
  byte a = SPI.transfer (what);
  delayMicroseconds (20);
  return a;
} // end of transferAndWait

int fetch_hymera_data(void)
{
  // enable Slave Select
  digitalWrite(mega_168_cs, LOW);  
  
  transferAndWait('a');  //fetch count command
  byte count = transferAndWait(0);
  digitalWrite(mega_168_cs, HIGH);    
 
 // Serial.print("count=");
 // Serial.println (count, DEC);
 
  return (int)count;
 
}  
