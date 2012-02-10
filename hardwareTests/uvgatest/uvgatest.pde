
#include <NewSoftSerial.h>

NewSoftSerial mySerial(2, 3);

void setup()  
{
  Serial.begin(9600);

  // set the data rate for the NewSoftSerial port
  mySerial.begin(9600);

  Serial.println( "autobaud" );
  mySerial.print( "U" );
 
  getResponse();
 
  
  Serial.println( "pensize = solid" );
  mySerial.print( 0x70);
  mySerial.print( 0, HEX );
  getResponse();

  Serial.println( "screen res" );
   
  mySerial.print( "Y" ); //display func
  mySerial.print( 0x0C, HEX ); //rsolution
  mySerial.print( 0x01, HEX ); //640 x 480

  getResponse();
    
    Serial.println( "baud" );
  mySerial.print( "Q" ); //baud
  sendHex( 0x0D );
  delay(1000);
  getResponse();
  
  mySerial.begin(115200);
  
 // drawRect( 10,10, 50,50);
/*
for( int i = 0; i < 600 ; i += 10 )
{
  drawRect( i );

  //getResponse(); 
}
*/
}

void sendHex(byte val )
{
  mySerial.print( val);
}

void sendDB( int i )
{

 mySerial.print( (byte) ((i >> 8) & 0xFF) );
 mySerial.print( (byte) (i & 0xFF) ); 
}
    
  
    

void drawRect( int x1, int y1, int x2, int y2 )
{
  sendHex(0x72);
 
 sendDB(x1); //x1
 sendDB(y1); //y1
 sendDB(x2); //x2
 sendDB(y2); //y2
 sendDB(1000); //colour
}
void drawCircle()
{
  Serial.println( "circle 2 " );
  sendHex(0x43);
  sendHex(0x00);

  sendHex(0x00);
  sendHex(0x3F);
  sendHex(0x00);
  sendHex(0x22);
  sendHex(0x00);
  sendHex(0x1F);
}

void getResponse()
{
  delay(150);
  while (mySerial.available()) {
      Serial.print((char)mySerial.read(), HEX);
  }
  Serial.println("");
}
void loop()                     // run over and over again
{

  /*
  if (mySerial.available()) {
      Serial.print((char)mySerial.read(), HEX);
  }
  if (Serial.available()) {
      mySerial.print((char)Serial.read());
  }
  */
  int rx = analogRead(A0 );
  int ry = analogRead(A1 );
  int x = (int)map(rx,0,1024,0,640);
  int y = (int)map(ry,0,1024,0,480);
  Serial.println( x );
  Serial.println( y );
  int rectSize = 5;
  drawRect( x, y, x+ rectSize, y + rectSize);
}
