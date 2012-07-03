#include <NewSoftSerial.h>
#define CC_RX 4
#define LED_PIN 6
NewSoftSerial ccSerial(CC_RX,NULL);


char startPwr[] = "<ch1><watts>";
char startTmpr[] = "<tmpr>";
char endChar = '<';

char readChar = 0xFF;

int sizePwr = sizeof(startPwr)-1;
int sizeTmpr = sizeof(startTmpr)-1;

int statePwr = sizePwr;
int stateTmpr = sizeTmpr;

int newstate = 0;

long PwrNum = 0;
double TmprDouble = 0;
long TmprNum = 0;
unsigned int PwrSize = 0;
unsigned int TmprSize = 0;
unsigned int DataSize = 0;

char Pwr[16] = "";
char Tmpr[16] = "";
char TmprBuffer[16] = "";
char PwrBuffer[64] = "";
char DataSizeBuffer[8] = "";

void setup()
{
    Serial.begin(9600);
    Serial.println( "started" );
  // Set the data rate for the SoftwareSerial port
  ccSerial.begin(9600);
  
}

void loop()
{
    if( ccSerial.available() )
    {
   readChar = ccSerial.read();
  if (readChar > 31) {
    //stateTmpr = parseData2(stateTmpr, sizeTmpr, startTmpr, readChar);
    stateTmpr = parseDataTmpr(stateTmpr, readChar);
    if (stateTmpr < 0) {
       //Serial.print(readChar);
       Tmpr[abs(stateTmpr)-1] = readChar;
    }
    //statePwr = parseData2(statePwr, sizePwr, startPwr, readChar);
    statePwr = parseDataPwr(statePwr, readChar);
    if (statePwr < 0) {
       //Serial.print(readChar);
       Pwr[abs(statePwr)-1] = readChar;
    }
  } else if (readChar == 13) {
    
    digitalWrite(LED_PIN, HIGH);
    
    PwrNum = atol(Pwr);
    TmprDouble = strtod(Tmpr,NULL);
    Serial.flush();
      Serial.print("Pwr: ");
      Serial.print(PwrNum);
      Serial.print(" ");
      Serial.print(" Tmpr: ");
      Serial.print(TmprDouble);
      Serial.println(".");
    Serial.flush();
      
    // not necessary using, overwrite onset
    //for (int i=0; i<sizeof(Pwr); i++) Pwr[i] = '\0';
    //for (int i=0; i<sizeof(Tmpr); i++) Tmpr[i] = '\0';    
    // if reseting required use this instead
    memset(Pwr,255,sizeof(Pwr));
    memset(Tmpr,255,sizeof(Tmpr));
    
    digitalWrite(LED_PIN, LOW);
   } 
  }
 }

 int parseData2(int oldstate, int size, char *start, char chr) {
  newstate = oldstate;
  if (newstate > 0) {
    if (chr == start[size-newstate]) {
      newstate--;
    } else {
      newstate = size;
    }
  } else if (newstate <= 0) {
    newstate--;
    if (chr == endChar) {
      newstate = size;
    }
  }
  return newstate;
}

int parseDataTmpr(int oldstate, char chr) {
  newstate = oldstate;
  if (newstate > 0) {
    if (chr == startTmpr[sizeTmpr-newstate]) {
      newstate--;
    } else {
      newstate = sizeTmpr;
    }
  } else if (newstate <= 0) {
    newstate--;
    if (chr == endChar) {
      newstate = sizeTmpr;
    }
  }
  return newstate;
}
int parseDataPwr(int oldstate, char chr) {
  newstate = oldstate;
  if (newstate > 0) {
    if (chr == startPwr[sizePwr-newstate]) {
      newstate--;
    } else {
      newstate = sizePwr;
    }
  } else if (newstate <= 0) {
    newstate--;
    if (chr == endChar) {
      newstate = sizePwr;
    }
  }
  return newstate;
}

