/* 
TODO:
fix motor on at pc boot
*/
#define ZC 2
#define STATUS_LED 12
#define RUN_LED 11
#define TRIAC 3
#define SPEED A0
#define RPM 5
#include <PID_v1.h>
#define EXT_RUN A1
#define EXT_SPD A2
#include <LiquidCrystal.h>
#define SAFE_OFF_TIME 10000
LiquidCrystal lcd(4, 6, 7, 8, 9, 10);

boolean usePID = true;
boolean run = false;
boolean led_status;
//closet to green led: blue red white green
int pulse_length;
int min_pulse;
int max_pulse;
double target_rpm, Input, Output;
PID myPID(&Input, &Output, &target_rpm,0.007,0.0006,0,REVERSE);

void setup()
{
  
  target_rpm = 0;
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(100);
  //had to load these in again, as they didn't seem to take on the contructor above.
  myPID.SetTunings(0.007,0.006,0);
  
  pinMode(RPM,INPUT);
//  digitalWrite(RPM,HIGH);
  pinMode(EXT_RUN,INPUT);
  digitalWrite(EXT_RUN,LOW);

  pinMode(TRIAC,OUTPUT);
  digitalWrite(TRIAC,LOW);
  pinMode(ZC,INPUT);

  pinMode(RUN_LED,OUTPUT);
  pinMode(STATUS_LED,OUTPUT);
  digitalWrite(STATUS_LED,HIGH);
  Serial.begin(115200);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("motor pid");


 Serial.println("disable spindle for SAFE_OFF_TIME");
  delay(SAFE_OFF_TIME);
  
  noInterrupts();           // disable all interrupts

  //the following will stop delay & millis from working
  //setup for the RPM counter on timer 1
  //clear the control registers
  TCCR1A = 0;
  TCCR1B = 0;
  
  TCCR1B |= (1 << CS12) | ( 1<< CS11 ); //external pin 5 count
  TCCR1B |= (1 << ICNC1 ); //noise canceller

  //setup for the triac control on timer 2
  TCCR2A = 0;
  TCCR2B = 0;
  
  //all this is using timer2
  TCCR2B |= (1 << CS22) | (1<< CS21) | (1<<CS20);    // 1024 prescaler 
 

  //want this to vary between 0 and 10ms (100hz)
  //16mhz / 1024 / 100hz = 156
  //156 will be the slowest, 0 the fastest
  //so timer-reset varies from 0 to 156
  max_pulse = 140; //little shorter, so never off.
  min_pulse = 1;
  pulse_length = max_pulse;
  myPID.SetOutputLimits(min_pulse,max_pulse);
  myPID.Compute();

  
  attachInterrupt(0,ZC_INT,FALLING); //set the interrupt handler for the RPM counter
  disableSpindle();
 
    
}

void enableSpindle()
{
   TIMSK2 |= (1 << TOIE2);   // enable timer  2 overflow interrupt
   interrupts();
}
void disableSpindle()
{
  TIMSK2 |= (0 << TOIE2);   // disable timer  2 overflow interrupt  
  digitalWrite(TRIAC,LOW);
  noInterrupts();

}

//timer 2 overflow interrupt - used to turn on the triac
ISR(TIMER2_OVF_vect)        
{
 digitalWrite(TRIAC,HIGH);
}

//zc interrupt
void ZC_INT()
{
  digitalWrite(TRIAC,LOW);
  TCNT2 = 256 - pulse_length;   // preload timer
}

double last_print = 0;
void loop()
{
  if(Serial.available())
  {
    double p = float(serReadInt())/1000.0;
    double i = float(serReadInt())/1000.0;
    double d = float(serReadInt())/1000.0;
    
    Serial.println(p,5);
    Serial.println(i,5);
    Serial.println(d,5);
    myPID.SetTunings(p,i,d);
  }  
   //delay(100);
  //   target_rpm = map(analogRead(SPEED),0,1024,0,30000);
  target_rpm = map(analogRead(EXT_SPD),0,1000,5000,30000);
  run = digitalRead(EXT_RUN);
  if( run )
  {
    enableSpindle();
    digitalWrite(RUN_LED,HIGH); 

   myPID.Compute();
   if(usePID)
     pulse_length = Output;
   else
     pulse_length = map(analogRead(EXT_SPD),1024,0,min_pulse,max_pulse);

   //pulse_length = map(analogRead(SPEED),0,1024,min_pulse,max_pulse);
  }
  else
  {
    //safely off 
    disableSpindle();
   digitalWrite(RUN_LED,LOW);
   
    pulse_length = max_pulse;
  }

  if( millis() - last_print > 250)
  {
      Input = getRPM();
          last_print = millis();


   // Serial.print(run ? "1\t" : "0\t" );
 //   Serial.print(last_print);
 //   Serial.print("\t");
    Serial.print(target_rpm);
    Serial.print("\t");
 //  Serial.print("rpm : " );
    Serial.println(Input);
    
  }
}

//if 50 counts in 100ms, then that's 500 counts in 1s
//500 * 60 = 30000RPM
double last_get = 0;

float getRPM()
{
  double now = millis();  

  int count = TCNT1;
  TCNT1 = 0;
 
  int interval = now - last_get;
 // Serial.print("count:"); Serial.println(count);
  //Serial.print("interval:"); Serial.println(interval);
  float rpm = (60000 * count / interval );
  
  last_get = now;  
  return rpm;
}

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

