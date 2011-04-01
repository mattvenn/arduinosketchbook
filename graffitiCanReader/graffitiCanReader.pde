#include <NewSoftSerial.h>
#define USB_TX 3
#define USB_RX 2

NewSoftSerial radio(USB_TX, USB_RX);

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode( 13, OUTPUT);

  pinMode( USB_RX, INPUT );
  digitalWrite( USB_RX, LOW );
  pinMode( USB_TX, INPUT );
  digitalWrite( USB_TX, LOW );
  Serial.println( "getting radio data..." );

  radio.begin(2400);
 
}

void loop() {

  if( radio.available() )
  {
Serial.println( "radio data available" );
    digitalWrite( 13, HIGH );
    byte nozz, dist, cksum;
    byte startByte = (byte)radio.read();
 Serial.println( startByte, HEX );
    if( startByte == 0xAA )
    {
Serial.println( "got start byte" );
      while( ! radio.available() ) {}
      nozz = (byte)radio.read();
      while( ! radio.available() ) {}
      dist = (byte)radio.read();
      while( ! radio.available() ) {}
      cksum = (byte)radio.read();
     /* 
      Serial.println( nozz, HEX);
      Serial.println( dist, HEX);
      Serial.println( cksum, HEX );
      */
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
               digitalWrite( 13, LOW );
    }

  }
}
