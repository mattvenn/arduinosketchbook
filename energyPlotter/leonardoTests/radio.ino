#ifdef testRadio


byte needToSend;
//payload def
typedef struct {
  byte servo;
  int stepperL;
  int stepperR;

} Payload;
Payload payload;

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

void doRadio()
{
 if (rf12_recvDone() && rf12_crc == 0 and rf12_len == sizeof(Payload))
  {
    const Payload* p = (const Payload*) rf12_data;
      digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    Serial.print( "got data: " );
    Serial.println( p->servo );
    Serial.println( p->stepperL );
        Serial.println( p->stepperR );
   /*
    if( p->servo != 0 )
    {
     // myservo.write(p->servo );

    }

    if( p->stepperL != 0 )
{
    analogWrite( STEP_PWM, PWM_HIGH );           
      leftStepper.step(p->stepperL);
         analogWrite( STEP_PWM, PWM_LOW );
        Serial.println( "pwm is low" );
}
    if( p->stepperR != 0 )
{
    analogWrite( STEP_PWM, PWM_HIGH );           
      rightStepper.step(p->stepperR);
         analogWrite( STEP_PWM, PWM_LOW );
        Serial.println( "pwm is low" );
}
    */

  }
 

  //send data once 
  if (sendTimer.poll(500))
  {
   
    needToSend = 1;
   
    Serial.println( "data ready to send:" );
  
  
  }
  if (needToSend && rf12_canSend())
  {

    needToSend = 0;
    //broadcast
    payload.servo = 0;
    payload.stepperL = 10;
    payload.stepperR = 20;
    
    rf12_sendStart(0, &payload, sizeof payload);
    Serial.println("sent");
 
  }

}

#endif
