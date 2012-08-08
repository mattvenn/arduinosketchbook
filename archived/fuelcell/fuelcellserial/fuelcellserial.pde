#include <NewSoftSerial.h>

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
