//#define VERBP

#ifdef PARSE
int stripHeaders(int pos)
{

#ifdef VERBP
  Serial.println( "strip header" );
#endif
  while( char cur = Ethernet::buffer[pos++] )
  {
    //Serial.print( cur, DEC );
    //Serial.print(",");
    //Serial.println( Ethernet::buffer[pos], DEC );

    //find line break, which looks like \n \r \n \r, so we can do it by finding \n . \n 
    //remember [pos] is next char, so [pos+1] is 2 chars time...
    if( cur == '\n' && Ethernet::buffer[pos+1] == '\n' )
    {
#ifdef VERBP
      Serial.print("header ends at");
      Serial.println( pos + 2 );
#endif
      return pos + 2;

    }  
  }
}

int parse(int i)
{
  char numbuf[10];
  int line = 0;
#ifdef VERBP
  Serial.println( "parsing" );
#endif

  boolean gotCmd = false;
  int numi = 0; //index for number parsing
  int nums = 0; //how many numbers found
  char nextChar; //next char in the buffer

    while( char cur = Ethernet::buffer[i++] )
  {
    nextChar = Ethernet::buffer[i];
    //Serial.print( cur );
    //Serial.print( "," );
    // Serial.println( nextChar );
    if( cur == '\n' )
    {
#ifdef VERBP
      Serial.print( "finished line:" );
      Serial.println( line );
#endif
      line ++;
      if(line>=MAXCOMMANDQUEUE)
      {
         Serial.println("queue too long to process!!");
         return line;
      }
      nums = 0;
      gotCmd = false;

    }
    else if( gotCmd == false )
    {
      payload[line].command = cur;
    //  payload[line].index = line;
#ifdef VERBP
      Serial.print( "got cmd:" );
      Serial.println( cur );
#endif
      gotCmd = true;
    }
    //do we need this?
    else if( cur == 0 )
    {
      //finish
#ifdef VERBP
      Serial.print( "finished with:" );
      Serial.println( line );
#endif
      return line;
    }
    else if( cur == ',' )
    {
      // eat it
    }
    else //reading an arg
    {
#ifdef VERBP
      if( numi == 0 )
        Serial.print( "reading arg for line" );
       
#endif
      //Serial.println( "next char is " ); Serial.println( nextChar );
      //Serial.println( numi );

      numbuf[numi++] = cur;   
      if( nextChar == ',' || nextChar == '\n' )
      {
        numbuf[numi] = 0;
        if( nums == 0 )
          payload[line].arg1 = atoi(numbuf);
        else if( nums == 1 )
          payload[line].arg2 = atoi(numbuf);
        nums ++;
        //Serial.print( "finished num:" );
#ifdef VERBP
        Serial.println( numbuf );
#endif
        numi = 0;
        numbuf[0] = 0;
      }
    }   
  } 
#ifdef VERBP
  Serial.print( "finished with:" );
  Serial.println( line );
#endif
 
  return line;
}

#endif
