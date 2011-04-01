#include <NewSoftSerial.h>

#include "WProgram.h"
void setup();
void loop();
void setupFuelCellSerial();
unsigned char getStatus();
unsigned char getStackV();
int getCurrent();
int updateFuelCell();
NewSoftSerial usb(5,6);

void setup()
{
    setupFuelCellSerial(); //on pin 13

    usb.begin( 9600 );
    Serial.begin( 9600 );
}

void loop()
{
  
 for( int i = 0 ; i < 100 ; i ++ )
 {
     //check usb stuff
     if( usb.available() )
     {
           //do something
     }
 }
 while( 1 )
 {
     //wait for fuelcell
     if( updateFuelCell() ) break;
 }
 
       Serial.println( "got new fuel cell data:" );
       Serial.print( "stack voltage: " );
       Serial.println( getStackV(), DEC );
       Serial.print( "current: " );
       Serial.println( getCurrent(), DEC );
   
 
}
#include <NewSoftSerial.h>
#define RXPIN 13
NewSoftSerial fuelcellSerial(RXPIN, 0); //tx, rx

typedef struct {
  unsigned char status;
  unsigned char stackT;
  unsigned char stackV;
  unsigned char stackI;
  unsigned char stackT2;
  unsigned char stackT3;
  int current;
  boolean updated;
} fuelCellData;

fuelCellData fuelcell;

void setupFuelCellSerial()  
{
  // set the data rate for the NewSoftSerial port
    fuelcellSerial.begin(9600);

}

//needs to be sorted out
unsigned char getStatus()
{
   return fuelcell.status;
}
unsigned char getStackV()
{
  return fuelcell.stackV;
}
int getCurrent()
{
  return fuelcell.current;
}

//returns 1 if new data has arrived
int updateFuelCell()
{
  //if data available, update the fuelcell structure
  if (fuelcellSerial.available() > 8 ) 
  {
       Serial.println( "new fuel cell data!" );
       fuelcell.status = fuelcellSerial.read();
       fuelcell.stackT = fuelcellSerial.read();
       fuelcell.stackV = fuelcellSerial.read();
       fuelcell.stackI = fuelcellSerial.read();
       fuelcell.stackT2 = fuelcellSerial.read();
       fuelcell.stackT3 = fuelcellSerial.read();
       fuelcell.current = fuelcellSerial.read() * 256; //high byte
       fuelcell.current += fuelcellSerial.read(); //low byte
       fuelcell.updated = HIGH;
       return 1;
  }
  return 0;
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

