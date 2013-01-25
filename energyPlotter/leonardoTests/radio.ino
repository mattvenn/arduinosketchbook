#ifdef useRadio
byte needToSend;


void cleanSPIBus()
{
  //don't understand this, but necessary for radio to work after an SD write
  digitalWrite(RFM_SEL,LOW);
  SPI.transfer(0x00);
  digitalWrite(RFM_SEL,HIGH);
}
void initRadio()
{
  rf12_initialize(1, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
}
/*
void testRadioPins()
 {
 Serial.println("radio miso: ");
 Serial.println( digitalRead( MISO ) ? "1" : "0" );
 digitalWrite(RFM_SEL,LOW);
 // digitalWrite(MISO,HIGH);
 delay(200);
 //  digitalWrite(MISO,LOW);
 //   delay(200);
 
 }
 */
void doRadio()
{
  if (rf12_recvDone())
  {
    if( rf12_crc == 0 )
    {
      if( rf12_len == sizeof(Payload))
      {

        Payload* p = (Payload*) rf12_data;
        /* digitalWrite(led,HIGH);
         delay(100);
         digitalWrite(led,LOW);*/
        Serial.print( "got data: " );
        printPayload(p);


        if( p->id == id ) //for us
        {
          //terrid - fixme
          payload.command = p->command;
          payload.arg1 = p->arg1;
          payload.arg2 = p->arg2;
          payload.id = p->id;
          commandWaiting = true;
        }
        else
        {
          Serial.print( "ignoring packet for: ");
          Serial.println(p->id,DEC);
        }
      }
      else
      {
        Serial.println("data wrong length");
      }

    }
    else
    {
      Serial.println("bad radio crc");
    }
  }



  if (sendAck && rf12_canSend())
  {
    sendAck = false;
    rf12_sendStart(0, &payload, sizeof payload);
    //   Serial.println("sent ack"); 
  }

}

#endif

