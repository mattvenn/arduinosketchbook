#define ZC 2
#define STATUS_LED 12
#define RUN_LED 11
#define TRIAC 3
#define SPEED A0
#define RPM 5

int pulse_length;
void setup()
{
    noInterrupts();           // disable all interrupts
  pinMode(RPM,INPUT);
  digitalWrite(RPM,LOW);
  pinMode(TRIAC,OUTPUT);
  digitalWrite(TRIAC,LOW);
  pinMode(ZC,INPUT);

  pinMode(RUN_LED,OUTPUT);
  pinMode(STATUS_LED,OUTPUT);
  digitalWrite(STATUS_LED,HIGH);
Serial.begin(9600);
TCCR1A = 0;
  TCCR1B = 0;

  
  

  TCCR1B |= (1 << CS11) | (1 << CS10);    // 64 prescaler 
//TCCR1B |= (1 << CS12);
 //want this to vary between 0 and 10ms (100hz)
// 16mhz / 64 / 100hz = 2500
  //so timer-reset varies from 0 to 2500
  pulse_length = 10;
  attachInterrupt(0,ZC_INT,FALLING);
   TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    interrupts();             // enable all interrupts

  
}
boolean led_status;
ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
 digitalWrite(RUN_LED,LOW);
 digitalWrite(TRIAC,HIGH);
 //delayMicroseconds(10);
// digitalWrite(TRIAC,LOW);

}

void ZC_INT()
{
  digitalWrite(TRIAC,LOW);
  digitalWrite(RUN_LED,HIGH); 
  TCNT1 = 65535 - pulse_length;   // preload timer
}

void loop()
{
/* pulse_length += 1;
 if( pulse_length >= 2500) 
   pulse_length = 30;
   delay(5);
   */
   pulse_length = map(analogRead(SPEED),0,1024,0,2500);
   Serial.println(pulse_length);
   delay(100);
   if(digitalRead(RPM))
       digitalWrite(STATUS_LED,HIGH); 
   else
       digitalWrite(STATUS_LED,LOW); 

   Serial.println(digitalRead(RPM));
}
