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
 if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
   /* digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);*/
    Serial.print( "got data: " );
    Serial.print( p->command ); 
    Serial.print(":");
    Serial.print( p->arg1 );
    Serial.print(":");
    Serial.println( p->arg2 );
    //terrid - fixme
    payload.command = p->command;
    payload.arg1 = p->arg1;
    payload.arg2 = p->arg2;
    
    commandWaiting = true;
  }
 

  if (sendAck && rf12_canSend())
  {
    sendAck = false;
    rf12_sendStart(0, &payload, sizeof payload);
//    Serial.println("sent"); 
//        delay(100);
  }

}

#endif
