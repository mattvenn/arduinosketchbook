#ifdef XBEE
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
      Serial.print( string, DEC );
      Serial.print( ":got val:" );
      Serial.println( val );
      switch ( string )
      {
        case 0:
          energy = val;
          break;
          /*
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
          */
      }
      string ++;
      inString = "";
    }
  }
}
  
#endif
