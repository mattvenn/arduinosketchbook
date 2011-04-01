#include <NewSoftSerial.h>

#define RX 4
#define TX 5
#define BUSY 6
#define DE 7 //pin 3
NewSoftSerial pot(RX,TX, true);
//NewSoftSerial potInvert( RX,TX, true );
void setup()
{
  Serial.begin( 9600 );
  pinMode( RX, INPUT );
    pinMode( TX, OUTPUT );

    pinMode( DE, OUTPUT );
    pinMode( BUSY, OUTPUT );
    digitalWrite( DE, LOW );
    
  Serial.println("starting");
  pot.begin( 9600 );

}

//this only works for the hard coded serial number below
void setAddress()
{
delay( 100 );
 digitalWrite( DE, HIGH );
  delay(1);
  pot.print( 0xF0, BYTE ); //request byte
  pot.print( 0x07, BYTE ); //get address
  pot.print( 0x00, BYTE );  //serial byte 1
  pot.print( 0x00, BYTE );  //b2
  pot.print( 0x3F, BYTE );  //b3
  pot.print( 0x5C, BYTE );  //b4
  pot.print( 0x01, BYTE );  // new address
//  0x3F5C
  digitalWrite( DE, LOW);
if(pot.available() == 1 )
{
//   Serial.print( pot.read(), HEX );
   Serial.println( pot.read(), HEX );
}  
  pot.flush();
}

//only works for hard coded serial below
void getAddress()
{
delay( 100 );
 digitalWrite( DE, HIGH );
  delay(1);
  pot.print( 0xF0, BYTE ); //request byte
  pot.print( 0x06, BYTE ); //get address
  pot.print( 0x00, BYTE );
  pot.print( 0x00, BYTE );
  pot.print( 0x3F, BYTE );
  pot.print( 0x5C, BYTE );
//  0x3F5C
  digitalWrite( DE, LOW);
if(pot.available() == 2 )
{
   Serial.print( pot.read(), HEX );
   Serial.println( pot.read(), HEX );
}  
  pot.flush();
}
  


int getPos( byte add)
{
// delay( 1 );

  byte i = 0x10;
  i += add;
  
  digitalWrite( DE, HIGH );
  delay(1);
  pot.print( i, BYTE ); //, BYTE );
  digitalWrite( DE, LOW);
  int count = 0;
  while(( pot.available() < 2 ) && (count < 1000 ) )
  {
    count ++;
  }
  int val = -1;
  if(pot.available() == 2 )
{
    
 
   val = pot.read();
   val *= 255;
   val += pot.read();
/*
  Serial.print( "dev: " );
  Serial.print( add, DEC );
  Serial.print( ": " );
  Serial.println( val );*/
}  
/*
else
{
  Serial.println( pot.available(), DEC );
}
*/
  pot.flush();
  delay( 5 );
 return val;

}

int enc0 = 0;
int enc1 = 0;
void loop()
{
//Serial.println( "sent..");
// setAddress();
 //getAddress();
 int enc;
 enc = getPos(0);
 if( enc >= 0 )
  enc0 = enc;
  delay( 5 );
 enc = getPos( 1 );
 if( enc >= 0 )
  enc1 = enc;
 delay( 5 );

char outStr[10];
 sprintf( outStr, "%04d %04d", enc0, enc1 );
 Serial.println( outStr );
}
