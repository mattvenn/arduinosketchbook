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

//check this when using optos with correct resistors
//I expected this to work with digitalRead but it doesn't seem to.
boolean limit( int input )
{
  if( analogRead( input ) > 1000 )
    return true;
  return false;
}
