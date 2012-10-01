int serReadInt()
{
  int i, serAva;                           // i is a counter, serAva hold number of serial available
  char inputBytes [7];                 // Array hold input bytes
  char * inputBytesPtr = &inputBytes[0];  // Pointer to the first element of the array

  if (Serial.available()>0)            // Check to see if there are any serial input
  {
    delay(5);                              // Delay for terminal to finish transmitted
    // 5mS work great for 9600 baud (increase this number for slower baud)
    serAva = Serial.available();  // Read number of input bytes
    for (i=0; i<serAva; i++)   
    {
      char readChar = (char)Serial.read();   // Load input bytes into array
      if( readChar == ',' )
        break;
      inputBytes[i] = readChar;
    }
    inputBytes[i] =  '\0';             // Put NULL character at the end
    return atoi(inputBytesPtr);    // Call atoi function and return result
  }
  else
    return -1;                           // Return -1 if there is no input
}

void getSerial()
{
  Serial.println( F("type serial of the board, then press enter"));
  while( Serial.available() == 0 ) 
  {
    ;;
  }
  delay(100);
  Serial.println( Serial.available() );
  while (Serial.available() > 0) 
  {
    int inChar = Serial.read();
    if (inChar == '\n') 
    {
      break;
    }
    else
    {
       serialStr += (char)inChar; 
    }
  }
  Serial.print( F("got: "));
  Serial.println( serialStr );
}

void waitForBoard()
{
  Serial.println( F("type y when board is plugged in"));
  readYN();
}

int readYN()
{
  while( Serial.available() == 0 ) 
  {
    ;;
  }
  char a = Serial.read();
  Serial.flush();
  if( a == 'y' )
    return 1;
  return 0;
}

void buzz()
{
  for( int i = 2000; i < 4000; i += 500 )
  {
    tone(buzzerPin, i, 500 );
    delay(200);
  }
}
