#include <SoftwareSerial.h>

//#include <NewSoftSerial.h>
#define USB_TX 2
#define USB_RX 3

//NewSoftSerial radio(USB_TX, USB_RX);
#include "WProgram.h"
void setup();
void loop();
SoftwareSerial radio =  SoftwareSerial(USB_RX, USB_TX);

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode( 13, OUTPUT);

  pinMode( USB_RX, INPUT );
  digitalWrite( USB_RX, LOW );
  pinMode( USB_TX, INPUT );
  digitalWrite( USB_TX, LOW );
  for( int i = 0; i < 20; i ++ )
  {
  Serial.println( "getting radio data..." );
  }

  radio.begin(4800);
 
}

void loop() {

  //if( radio.available() )
 // {
//Serial.println( "radio data available" );
    digitalWrite( 13, HIGH );
    byte nozz, dist, cksum;
    byte startByte = (byte)radio.read();
 Serial.println( startByte  );
 
    if( startByte == 0xAA )
    {
Serial.println( "got start byte" );
    }
 /*
      while( ! radio.available() ) {}
      nozz = (byte)radio.read();
      while( ! radio.available() ) {}
      dist = (byte)radio.read();
      while( ! radio.available() ) {}
      cksum = (byte)radio.read();
      
      Serial.println( nozz, HEX);
      Serial.println( dist, HEX);
      Serial.println( cksum, HEX );
      
      byte tcksum = nozz + dist;
     // Serial.println( tcksum, HEX );
      if( cksum == tcksum )
      {
Serial.println( "cksum ok" );

       //  Serial.print( "cksum good : " ); //we have got a good data packet
         Serial.print( "noz: " );
         Serial.print( nozz, DEC );
         Serial.print( ", dist: " );
         Serial.println( dist, DEC );

      }

               
    }

  }
  */
                 digitalWrite( 13, LOW );
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

