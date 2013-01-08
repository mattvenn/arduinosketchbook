
int serReadInt()
{
  char inputBytes [7];                 // Array hold input bytes
  char * inputBytesPtr = &inputBytes[0];  // Pointer to the first element of the array
  if( serReadNumber(inputBytes) ) // a successful read
     return atoi(inputBytesPtr);    // Call atoi function and return result
  return 0; //failure  
}
float serReadFloat()
{
    char inputBytes [7];                 // Array hold input bytes
  char * inputBytesPtr = &inputBytes[0];  // Pointer to the first element of the array
  if( serReadNumber(inputBytes) ) // a successful read
     return atof(inputBytesPtr);    // Call atoi function and return result
  return 0; //failure  
}

int serReadNumber(char * inputBytes)  
{
  int i, serAva;                           // i is a counter, serAva hold number of serial available
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
    return i;
  }
  else
    return 0;                           // Return -1 if there is no input
}

void printPayload( Payload * p)
{
    Serial.print("id:");
    Serial.print( p->id,DEC );
    Serial.print(",");
    
    Serial.print( p->command ); 
    Serial.print(":");
    Serial.print( p->arg1 );
    Serial.print(":");
    Serial.println( p->arg2 );
}

byte getId()
{
  return EEPROM.read(idAddress);
}

#ifdef testMem
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
int free_memory;
  
    if((int)__brkval == 0)
        free_memory = ((int)&free_memory) - ((int)&__bss_end);
    else
        free_memory = ((int)&free_memory) - ((int)__brkval);
    return free_memory;
};
#endif

#ifdef testIO
  Serial.print( "ints: " );
  Serial.println( b );
  Serial.print( "gpio1:" );

  Serial.println( analogRead( GPIO1_IN ) );
  Serial.print( "gpio2:" );
  Serial.println( analogRead( GPIO2_IN ) );
  myServo.write( 0 );
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(MS1, HIGH );
  digitalWrite(MS2, HIGH );
  //  digitalWrite( GPIO1, HIGH);
  //  digitalWrite(GPIO2, HIGH );
  digitalWrite( SD_SEL, HIGH );
  digitalWrite( RFM_SEL, HIGH );

  digitalWrite( STEPL, HIGH );
  digitalWrite( STEPR, HIGH );
  digitalWrite( DIRL, HIGH );
  digitalWrite( DIRR, HIGH );

  Serial.println( digitalRead(LIMITL) ? "LIMITL 1" : "LIMITL 0" );
  Serial.println( digitalRead(LIMITR) ? "LIMITR 1" : "LIMITR 0" );  
  analogWrite(PWML, 100 );
  analogWrite(PWMR, 100 );

  delay(1000);       

  digitalWrite( STEPL, LOW );
  digitalWrite( STEPR, LOW );
  digitalWrite( DIRL, LOW );
  digitalWrite( DIRR, LOW );

  // wait for a second
  digitalWrite(led, LOW);
  digitalWrite(MS1, LOW );
  digitalWrite(MS2, LOW );
  //  digitalWrite( GPIO1, LOW);
  //  digitalWrite(GPIO2, LOW );
  digitalWrite( SD_SEL, LOW );
  digitalWrite( RFM_SEL, LOW );
  analogWrite(PWML, 10 );
  analogWrite(PWMR, 10 );
  // turn the LED off by making the voltage LOW

#endif

