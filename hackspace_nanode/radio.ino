 void doRadio()
 {
   if (readyToSend && rf12_canSend())
  {

    readyToSend = false;
    //broadcast
  
    rf12_sendStart(0, &payload[commandIndex], sizeof payload[commandIndex]);
   
    gotAck = false;
    Serial.print("> sent ");
    Serial.print( commandIndex ); //payload[commandIndex].index );
    Serial.print( ":" );
    Serial.print( payload[commandIndex].command );
      Serial.print( payload[commandIndex].arg1 );
      Serial.print( "," );
      Serial.println( payload[commandIndex].arg2 );
      sentData = millis();
   // commandIndex ++;
  }
  
  if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
    gotAck = true;
    Serial.print( "< robot:" );
    Serial.print( p->command );
    Serial.print( p->arg1 );
    Serial.print(",");
    Serial.println( p->arg2 );
    if( payload[commandIndex].command == p->command && payload[commandIndex].arg1 == p->arg1 && payload[commandIndex].arg2 == p->arg2 )
    {
      //then the robot has drawn that command
      commandIndex ++;
    }
  }
 }


