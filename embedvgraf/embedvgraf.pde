
#include <NewSoftSerial.h>
#include <Wire.h>
#include <PVision.h>

PVision ircam;
byte result;
int lineWidth = 5;


NewSoftSerial mySerial(2, 3);

void setup()  
{
  Serial.begin(9600);
  ircam.init();
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
   /*
  mySerial.print( "Y" );
  sendHex( 0x0C ); //rsolution
  sendHex( 0x01 ); //640 x 480
*/
  getResponse();
    
    Serial.println( "baud" );
  mySerial.print( "Q" ); //baud
  sendHex( 0x0D );
  delay(1000);
  getResponse();
  Serial.println( "poly");
  drawFatLine( 5, 20,20, 200,20);
  getResponse();
    Serial.println( "poly");
    drawFatLine( 5, 20,60, 20,200);
    getResponse();
      Serial.println( "poly");
      drawFatLine( 5, 40,40, 200,200);
      getResponse();
 //   getResponse();
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
    
  

  
void drawLine( int x1, int y1, int x2, int y2 )
{
  sendHex(0x4C);
  sendDB(x1); //x1
 sendDB(y1); //y1
 sendDB(x2); //x2
 sendDB(y2); //y2
 sendDB(1000); //colour
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
void drawCircle(int x, int y, int r)
{
  Serial.println( "circle 2 " );
  sendHex(0x43);
  
  sendDB(x);
  sendDB(y);
  sendDB(r);
  sendDB(1000);

  
}

void drawFatLine( float w, int x1, int y1, int x2, int y2 )
{

 // Serial.println( x2 - x1 );
  ///Serial.println( y2 - y1 );
  float alpha = atan2(  x2 - x1 ,  y2 - y1 );
 // Serial.println( alpha );
  int xp = (int) w * cos( alpha );
  int yp = (int) w * sin( alpha );
 // Serial.print( "xp:" );
 // Serial.println( xp );
 // Serial.print( "yp:" );
 // Serial.println( yp );
  drawPoly( x1 + xp, y1 - yp, x2 + xp, y2 - yp, x2 - xp, y2 + yp, x1 - xp, y1 + yp );
  drawCircle( x1, y1, w - 1 )  ;
}

void drawPoly( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )
{

                      
  sendHex(0x67);
  
  sendHex(0x04); //vertices
  
  sendDB(x1); //x1
  sendDB(y1); //y1

 sendDB(x2);
 sendDB(y2);

 sendDB(x3);
 sendDB(y3);

 sendDB(x4);
 sendDB(y4);

 sendDB(1000); //colour
}

void getResponse()
{
  delay(150);
  while (mySerial.available()) {
      Serial.print((char)mySerial.read(), HEX);
  }
  Serial.println("");
}
int oldx , oldy;
boolean stopSpray = false;
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
  
  result = ircam.read();
  
  if (result & BLOB1)
  {
/*
  Serial.print("BLOB1 detected. X:");
    Serial.print(ircam.Blob1.X);
    Serial.print(" Y:");
    Serial.print(ircam.Blob1.Y);
    Serial.print(" Size:");
    Serial.println(ircam.Blob1.Size);
  */  
  int x = (int)map(ircam.Blob1.X,0,1024,0,640);
  int y = (int)map(ircam.Blob1.Y,0,1024,0,480);
 
  if( stopSpray )
  {
    stopSpray = false;
    oldx = x;
    oldy = y;
  }
    drawFatLine( lineWidth, oldx, oldy, x, y);
  oldx = x;
  oldy = y;
  }
  else
  {
    stopSpray = true;
  }
 // int rx = analogRead(A0 );
 // int ry = analogRead(A1 );

}
