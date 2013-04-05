
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

void dumpSerialConfig()
{
  for( i = 0; i < sizeof(config); i ++ )
  {
    //Serial.write(config[i]);
  }
}

void loadSerialConfig()
{
  ;;
}

void saveConfig()
{
  EEPROM_writeAnything(0, config);
}

void loadConfig()
{
  EEPROM_readAnything(0, config);
}

//probably best to get rid of this and program it in from the command line. store configs as text files in the repo.
void load_default_config()
{
  //machine config
  config.stepsPerRevolution=200;
  config.id = 0;
  //machine dimensions
  config.stepsPerMM = 6.48; //measured rather than calculated. stepsPerRevolution / circumference;   
  config.motor_dist_mm = 510;
  config.hanger_l = 30;
  config.gw = 30 * config.stepsPerMM;  //gondola bolt width
  config.w= config.motor_dist_mm * config.stepsPerMM;
  config.h= 680*config.stepsPerMM;  //300mm tall
  config.top_margin = 150*config.stepsPerMM; //gondola design causes too much distortion above here.
  config.side_margin = config.top_margin;
  //stepper  settings
  config.default_pwm = 60;
  config.lowpower_pwm = 10;
  config.HOME_PWM_HIGH = 150;
  config.HOME_PWM_LOW = 15;
  config.HOME_SPEED = 3000;
  config.maxSpeed = 3000; //800;

 saveConfig();


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


