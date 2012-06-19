//help on getting xbee setup here
//http://tropicarduino.blogspot.com/2010/11/xbee-and-newsoftserial.html

NewSoftSerial xbeeSerial(XBEE_RX,XBEE_TX); //pin 5 is RX
int dataStart, adIn, numConv, idx;
int dataArray[3][4];
byte packet[35]; //Array size is 3*4*2+11

//format the data for the robot and send with nss
void sendRobotData(int energyData)
{
//  if( minutes >= 0 )
  {
    Serial.print( "sending to energy robot: " );


    Serial.print( "e" );
    Serial.print( energyData );
    Serial.print( "," );
    Serial.print( minutes );
    Serial.print( "," );
    Serial.println( minutes + energyData );

    //had problems with NSS, started working and don't know why :(
    xbeeSerial.print( "e" );
    xbeeSerial.print( energyData );
    xbeeSerial.print( "," );
    xbeeSerial.print( minutes );
    xbeeSerial.print( "," );
    xbeeSerial.println( minutes + energyData );
    
  }
}

void xbeeSetup()  
{
  xbeeSerial.begin(9600);
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
      //Serial.print( string, DEC );
      //Serial.print( ":got val:" );
      //Serial.println( val );
      switch ( string )
      {
      case 0:
      {
        irms = val;    
        break;
      }
      case 1:
        gasPulses = val;
        break;
      case 2:
        battv = val;
        break;
      case 3:
        temp = val;
        dataReady = true;

        Serial.println( "got data from remote monitor" );
        break;
      }
      string ++;
      inString = "";
    }
  }
}






