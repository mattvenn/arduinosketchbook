#include <JeeLib.h>
MilliTimer sendTimer;

#define LED 6
boolean readyToSend = false;
//payload def
typedef struct {
  char command;
  unsigned int arg1;
  unsigned int arg2;

}
Payload;
Payload payload;
int testNum = 0;
MilliTimer testTimer;
char buffer[200];
boolean gotOK = true;
void setup()
{
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  Serial.println( "sender" );

  Serial.println( "initialising radio" );
  delay(100);
  rf12_initialize(2, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
 
}
int offset = 0;
void loop()
{
  boolean gotData = false;
  int i = 0;
  if( Serial.available() )
  {
    while( i <58)
    {
      if( Serial.available() )
        buffer[i++] = Serial.read();
    }
    Serial.println( i );
    gotData = true;
    Serial.flush();
    offset = stripHeaders();

  }

  if( gotData && gotOK )
  {
   
    if( offset != 0 )
    {
        offset = parse( offset );
        if( offset != 0 )
        {
          Serial.println( payload.command );
          Serial.println( payload.arg1 );
          Serial.println( payload.arg2 );
          readyToSend = true;
        }
        else
        {
          gotData = false;
        }
    }
   
  }
      
  if (readyToSend && rf12_canSend())
  {
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    readyToSend = false;
    //broadcast
    rf12_sendStart(0, &payload, sizeof payload);
    Serial.println("sent");
    gotOK = false;

  }
  
  if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    Serial.print( "got data: " );
    Serial.println( p->command );
    Serial.println( p->arg1 );
    Serial.println( p->arg2 );
    //not got the ok, but this will do for now
    gotOK = true;
    
  }

}


