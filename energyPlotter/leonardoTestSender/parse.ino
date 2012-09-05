int stripHeaders()
{

  int i =0;
  while( char cur = buffer[i++] )
  {
    //find line break    
    if( cur == '\n' && buffer[i] == '\n' )
    {
      Serial.print("header ends at");
      Serial.println( i + 1 );
      return i + 1;

    }  
  }
}

int parse(int i)
{
  char numbuf[10];
  payload.command = 0;
  payload.arg1 = 0;
  payload.arg2 = 0;

  Serial.println( "parse" );

  boolean gotCmd = false;
  int numi = 0; //index for number parsing
  int nums = 0; //how many numbers found
  char nextChar; //next char in the buffer


    while( char cur = buffer[i++] )
  {
    nextChar = buffer[i];
    //Serial.print( cur );
    //Serial.print( "," );
    // Serial.println( nextChar );
    if( cur == '\n' )
    {
      Serial.println( "got line end" );
      
      return i;
      /*
          arg1 = 0;
       arg2 = 0;
       command = 0;
       nums = 0;
       
       gotCmd = false;
       */
    }
    else if( gotCmd == false )
    {
      //Serial.println( "got cmd" );
      payload.command = cur;
      gotCmd = true;
    }
    else if( cur == 0 )
    {
      //finish
      return 0;
    }
    else if( cur == ',' )
    {
      // eat it
    }
    else //reading an arg
    {
      //          Serial.println(  "reading arg" );
      //Serial.println( "next char is " ); Serial.println( nextChar );
      numbuf[numi++] = cur;   
      if( nextChar == ',' || nextChar == '\n' )
      {
        numbuf[numi] = 0;
        if( nums == 0 )
          payload.arg1 = atoi(numbuf);
        else if( nums == 1 )
          payload.arg2 = atoi(numbuf);
        nums ++;
        //Serial.print( "finished num: " );
        //Serial.println( numbuf );
        numi = 0;
        numbuf[0] = 0;
      }
    }   
  } 
}

