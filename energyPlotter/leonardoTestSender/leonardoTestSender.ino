#include <JeeLib.h>
MilliTimer sendTimer;

#define LED 5
boolean readyToSend = false;
//payload def
typedef struct {
  char command;
  int arg1;
  int arg2;

}
Payload;
Payload payload;

void setup()
{
  Serial.begin(9600);
  Serial.println( "sender" );

  Serial.println( "initialising radio" );
  delay(100);
  rf12_initialize(2, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
}

byte pos = 0;
void loop()
{
  if( Serial.available() )
  {
    //need to delay for serreadint to work
    delay(100);
    digitalWrite( LED, HIGH );
    payload.command = Serial.read();
    payload.arg1 = serReadInt();
    payload.arg2 = serReadInt();
    
      readyToSend = true;

      
      digitalWrite( LED, LOW );
    
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
  }

  if (readyToSend && rf12_canSend())
  {

    readyToSend = false;
    //broadcast
    rf12_sendStart(0, &payload, sizeof payload);
    Serial.println("sent");

  }

}


