
//http://tropicarduino.blogspot.com/2010/11/xbee-and-newsoftserial.html
#include <NewSoftSerial.h>

NewSoftSerial xbeeSerial(4,5); //pin 5 is RX
int dataStart, adIn, numConv, idx;
int dataArray[3][4];
byte packet[35]; //Array size is 3*4*2+11


void xbeeSetup()  
{
  xbeeSerial.begin(9600);

  adIn = 3;  //number of A/D inputs to Xbee
  numConv = 4; //number of A/D conversions per input
  dataStart = 11;  //index of first data byte LSB
}

void checkXbeeData()
{
  int string = 0;
  String inString;
  while( xbeeSerial.available() )
  {
    int in = xbeeSerial.read();
    if( isDigit( in ) || in == '.' )
      inString += (char)in;

    if( in == ',' || in == '\n' )
    {
      char buff[10];
      inString.toCharArray( buff, 10 );
      double val = strtod(buff,NULL);
      Serial.print( string, DEC );
      Serial.print( ":got val:" );
      Serial.println( val );
      switch ( string )
      {
        case 0:
          irms = val;
          break;
        case 1:
          gas = val;
          break;
        case 2:
          battv = val;
          break;
        case 3:
          temp = val;
          dataReady = true;
          break;
          
      }
      string ++;
      inString = "";
    }
  }
}
  

