// Written by Nick Gammon
// April 2011

#include "pins_arduino.h"

// what to do with incoming data

void setup_spi (void)
{

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
   //bit confusing this, because stuff takes an extra turn to come around.
  switch (c)
  {
  // no command? then this is the command
  case 0:
    command = c;
    SPDR = 0;
    break;
    
  // return how much we've counted
  case 'a':
    SPDR = count;
    break;
    

  } // end of switch

}  
