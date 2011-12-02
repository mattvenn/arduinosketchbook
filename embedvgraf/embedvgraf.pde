/*

Friispray embedded! BETA software!
JamJarCollective.com

embedded virtual graffiti using 4d systems uvga-ii and a pixart ir blob tracking camera (from wiimote)
Matthew Venn 2011: mattvenn.net

this software is licensed under the GPL.

TODO

+ add 4 point calibration
+ add something to make colours better
+ add some status lights
+ look at saving images on the uvga-ii's sdcard
+ how can we get pixart cameras without trashing wiimotes?

*/

#include <NewSoftSerial.h>
#include <Wire.h>
#include <PVision.h>
#include <TimedAction.h>

//timed action for a status message
TimedAction timedAction = TimedAction(5000,status);

//globals
PVision ircam;
byte result;
int lineWidth = 8;
int drawColour = 1000;
int oldx , oldy;
boolean stopSpray = false;

NewSoftSerial mySerial(2, 3);

void setup()  
{
  Serial.begin(9600);
  ircam.init();
  // set the data rate for the NewSoftSerial port
  mySerial.begin(9600);

  Serial.println( "autobaud" );
  sendHex( 0x55 );
  getResponse();

  Serial.println( "pensize = solid" );
  sendHex( 0x70);
  sendHex( 0 );
  getResponse();

  Serial.println( "screen res" );
  sendHex( 0x59);
  sendHex( 0x0C ); //resolution
  sendHex( 0x01 ); //640 x 480
  delay(1000);
  getResponse();

  Serial.println( "baud" );
  mySerial.print( "Q" ); //baud
  sendHex( 0x0C ); //115200 doesn't work for receive - too fast

  mySerial.begin(57600);  
  delay(500);
  clearBuffer();

  Serial.println( "control bar" );
  drawControlBar();
  Serial.println( "done" );
}

//primitives to send data to uvga ///////////////////
void sendHex(byte val )
{
  mySerial.print( val);
  //  Serial.print(val);
}

void sendDB( int i )
{
  mySerial.print( (byte) ((i >> 8) & 0xFF) );
  mySerial.print( (byte) (i & 0xFF) ); 
  //Serial.print( (byte) ((i >> 8) & 0xFF) );
  // Serial.print( (byte) (i & 0xFF) ); 
}

//waits for an ack or nack or timesout
void getResponse()
{
  int i = 0;
  //Serial.println( "wait for ack/nack" );
  while(i ++ < 5000)
  {
    if(mySerial.available())
    {
      char ack = ((char)mySerial.read());
      if( ack == 0x06 )
      {
        //ack
        return;
      }
      else if ( ack == 0x15 )
      {
        //nack
        return;
      }
    }
  }
  //if we get here then flow control is bust and we might crash the uvga
  Serial.println( "timed out" );
}

void clearBuffer()
{
  mySerial.flush();  
  //request version info
  sendHex( 0x56 );
  sendHex( 0x00 );
  //but don't wait for ack
  while( mySerial.available() )
  {
    Serial.print( mySerial.read(), HEX );
    Serial.print( "," );
  }
}


/////////////////////////////

//graphics commands ////////////////////////////////
void eraseScreen()
{
  sendHex( 0x45 );
  getResponse();
}

void drawChar( int x, int y, char c, int colour )
{
  sendHex( 0x74);
  sendHex(c);
  sendDB(x);
  sendDB(y);
  sendDB(colour);
  sendHex( 0x01 ); //width
  sendHex( 0x01 ); //height
  getResponse();
}

void drawLine( int x1, int y1, int x2, int y2 )
{
  sendHex(0x4C);
  sendDB(x1); //x1
  sendDB(y1); //y1
  sendDB(x2); //x2
  sendDB(y2); //y2
  sendDB(1000); //colour
  getResponse();
}

void drawRect( int x1, int y1, int x2, int y2, int colour)
{
  sendHex(0x72);
  sendDB(x1); //x1
  sendDB(y1); //y1
  sendDB(x2); //x2
  sendDB(y2); //y2
  sendDB(colour); //colour
  getResponse();
}

void drawCircle(int x, int y, int r, int colour)
{
  sendHex(0x43);
  sendDB(x);
  sendDB(y);
  sendDB(r);
  sendDB(colour);
  getResponse();
}

void drawPoly( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int colour )
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

  sendDB(colour); //colour
  getResponse();
}

// stuff that uses the graphics primitives ////////////////////
void drawFatLine( float w, int x1, int y1, int x2, int y2 , int colour)
{
  float alpha = atan2(  x2 - x1 ,  y2 - y1 );
  int xp = (int) w * cos( alpha );
  int yp = (int) w * sin( alpha );
  drawPoly( x1 + xp, y1 - yp, x2 + xp, y2 - yp, x2 - xp, y2 + yp, x1 - xp, y1 + yp, colour );
  drawCircle( x1, y1, w - 1, colour )  ;
}

void status()
{
  Serial.println( "ok" );
  clearBuffer();
}

void loop()    
{
  timedAction.check();
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
    int y = (int)map(ircam.Blob1.Y,0,768,0,480);

    if( checkControlBar( x, y ) )
    {
      //    Serial.println( "in control area");
    }
    else
    {
      //  Serial.println( "painting" );
      //we're spraying
      if( stopSpray )
      {
        stopSpray = false;
        oldx = x;
        oldy = y;
      }

      drawFatLine( lineWidth, oldx, oldy, x, y, drawColour);

      oldx = x;
      oldy = y;
    }
  }
  else
  {
    stopSpray = true;
  }
}

