
    
void ntpRequest()
{
  Serial.print("Send NTP request ");
      Serial.println( currentTimeserver, DEC );
      es.ES_client_ntp_request(buf,timeServer[currentTimeserver++], clientPort);
      if( currentTimeserver >= NUM_TIMESERVERS )
        currentTimeserver = 0; 
}
 
 
void printRTCTime()
{
  DateTime now = RTC.now();
    Serial.print( "RTC: ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
   
    //update global minutes 
    minutes = now.hour() * 60 + now.minute();
 
}

//see if we have an NTP response
void checkNTP()
{

  dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));
    // Has unprocessed packet response
    if (dat_p > 0)
    {
      if (es.ES_client_ntp_process_answer(buf,&time,clientPort)) {
        Serial.print("got NTP update - time:");
        Serial.println(time); // secs since year 1900
       
        if (time) {
          time -= SECS_YR_1900_2000;
          DateTime now(time);
          RTC.adjust(now);
          Serial.println( "adjusted RTC to NTP" );
        }
      }
    }

}
