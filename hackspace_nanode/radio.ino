 void doRadio()
 {
   if (readyToSend && rf12_canSend())
  {

    readyToSend = false;
    //broadcast
  
    rf12_sendStart(0, &payload[commandIndex], sizeof payload[commandIndex]);
    gotAck = false;
    commandIndex ++;

    Serial.println("sent");
    Serial.print( payload[commandIndex].command );
      Serial.print( payload[commandIndex].arg1 );
      Serial.print( "," );
      Serial.println( payload[commandIndex].arg2 );

  }
  
  if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
    gotAck = true;
    Serial.println( "robot ack" );
//    Serial.println( p->command );
//    Serial.println( p->arg1 );
//    Serial.println( p->arg2 );
  }
 }


