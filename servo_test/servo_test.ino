
/* stuff learnt
arduino can et interupts at 100khz
but input wave forms must be nice and square - used a comparitor.
*/
volatile long int counter = 0;
#define EN 3
#define FOR 4
#define REV 5


#include <PID_v1.h>

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,4,4,0.1, DIRECT);
int countloop = 0;
void setup()
{
  Serial.begin(9600);
   attachInterrupt(0, count, FALLING);
   pinMode(EN,OUTPUT);
   pinMode(FOR,OUTPUT);
   pinMode(REV,OUTPUT);
   digitalWrite(EN,HIGH);
   digitalWrite(REV,LOW);
   digitalWrite(FOR,HIGH);
   
    //initialize the variables we're linked to
  Input = 0;
  Setpoint = 10;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}
void loop()
{
  delay(10);
  if(Serial.available())
  {
    Setpoint = serReadInt();
    Serial.print("set point:"); Serial.println(Setpoint);
}
  if(countloop++>100)
  {
  Serial.print("count:" ); Serial.println(counter);
  Serial.print("op:"); Serial.println(Output);
  countloop = 0;
  }
  
  Input = counter;
  
  myPID.Compute();
  analogWrite(3,Output);
  counter = 0;


}

void count()
{
  counter++;
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


