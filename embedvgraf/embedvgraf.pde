
#include <NewSoftSerial.h>
#include <Wire.h>
#include <PVision.h>
#include <TimedAction.h>

//this initializes a TimedAction class that will change the state of an LED every second.
TimedAction timedAction = TimedAction(5000,status);



PVision ircam;
byte result;
int lineWidth = 3;
int drawColour = 1000;

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

 
   /*
  Serial.println( "screen res" );
  mySerial.print( "Y" );
  sendHex( 0x0C ); //rsolution
  sendHex( 0x01 ); //640 x 480
  getResponse();
*/



  Serial.println( "baud" );
  mySerial.print( "Q" ); //baud
  sendHex( 0x0C ); //115200 doesn't work for receive - too fast

//  getResponse(); doesn't work after baud change

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

//blocks for an ack or nack
void getResponse()
{
  int i = 0;
  //Serial.println( "wait for ack/nack" );
  while(i ++ < 5000)
  {
    if(mySerial.available())
    {
      char ack = ((char)mySerial.read());
    //  Serial.print( "c:" );
    //  Serial.println( ack, HEX );
      
      
      if( ack == 0x06 )
      {
     //   Serial.println( "got ack" );
        return;
      }
      else if ( ack == 0x15 )
      {
      //  Serial.println( "got nack" );
        return;
      }
    }
  }
  Serial.println( "timed out" );
}
void clearBuffer()
{
    mySerial.flush();  
    //send a command
    sendHex( 0x56 );
    sendHex( 0x00 );
     //but don't wait for ack
    while( mySerial.available() )
    {
      Serial.print( mySerial.read(), HEX );
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

 // debug( x2 - x1 );
  ///debug( y2 - y1 );
  float alpha = atan2(  x2 - x1 ,  y2 - y1 );
 // Serial.println( alpha );
  int xp = (int) w * cos( alpha );
  int yp = (int) w * sin( alpha );
 // Serial.println( "xp:" );
 // Serial.println( xp );
 // Serial.println( "yp:" );
 // Serial.println( yp );
  drawPoly( x1 + xp, y1 - yp, x2 + xp, y2 - yp, x2 - xp, y2 + yp, x1 - xp, y1 + yp, colour );
  drawCircle( x1, y1, w - 1, colour )  ;
}

int oldx , oldy;
boolean stopSpray = false;

void status()
{
  Serial.println( "ok" );
  clearBuffer();
}
  

void loop()                     // run over and over again
{


  timedAction.check();
  

//  Serial.println( "loop");  
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
