#include <FiniteStateMachine.h>

//Constants
#define  secondsUnclip 5  // after refuelling is complete, wait this long before entering Ready state for next refuel - allows time to unclip syringe
#define  BAUDRATE  9600  
#define ledPause 100
//Pinouts
#define  button  12
#define  buttonLed 11
#define  led1  4
#define  led2  5
#define  led3  6
#define  led4  7
#define  led5  8
#define  led6  10

#define  fullSwitchPin 2
#define  syringePin 3 //pin 11 is blown cos I put 10v in
#define  solenoidPin  13

#define refillSafetyTime 5000 //seconds
//initialize states
State Ready = State(ledsOff);
State Refuelling = State(refuel);
State Waiting = State(waitUnclip);
FSM refuellerStateMachine = FSM(Ready);     //initialize state machine, start in state: Ready

volatile boolean filling = false;

void setup()  {

  pinMode (led1, OUTPUT);
  pinMode (led2, OUTPUT);
  pinMode (led3, OUTPUT);
  pinMode (led4, OUTPUT);
  pinMode (led5, OUTPUT);
  pinMode (led6, OUTPUT);
  pinMode (buttonLed, OUTPUT );
  pinMode (button, INPUT);      
  pinMode (fullSwitchPin, INPUT);
  pinMode (syringePin, INPUT);
  pinMode (solenoidPin, OUTPUT);
  
  //pullups
  digitalWrite( fullSwitchPin, HIGH );
  digitalWrite (button, HIGH);    // pulled up
  digitalWrite (solenoidPin, LOW);  // pin for electrolyser off
  Serial.begin (BAUDRATE);
  Serial.print ("H2 REFUELLER");

  Serial.print (" seconds    Wait for unclip: ");
  Serial.print (secondsUnclip);
  Serial.println (" seconds");
  
  //full switch interrupt
   attachInterrupt(0, fullSwitchInt, LOW);

}

void loop() {
    refuellerStateMachine.transitionTo(Refuelling);
    refuellerStateMachine.update();
    refuellerStateMachine.transitionTo(Waiting);
    refuellerStateMachine.update();
    refuellerStateMachine.transitionTo(Ready);
    refuellerStateMachine.update();
  }

//utility functions
void ledsOff() {          // Ready state - turn off LEDs and wait for button push

 digitalWrite(led1,LOW);
 digitalWrite(led2,LOW);
 digitalWrite(led3,LOW);
 digitalWrite(led4,LOW);
 digitalWrite(led5,LOW);
 digitalWrite(led6,LOW);
 
 while(true)
 {
   if( digitalRead(syringePin) )
   {
     digitalWrite(buttonLed, HIGH );
     if(digitalRead(button)==LOW ) //we have pull ups
       return;
   }
   else
   {
     digitalWrite(buttonLed, LOW );
   }
 }
 
 }

void fullSwitchInt()
{
  filling = false;
}
void refuel () {
  Serial.print("Refuelling ");
  digitalWrite(solenoidPin,HIGH);   // pin for electrolyser on
  filling = true;
  double startTime = millis();
  while( filling && ( ( millis() - startTime  ) < refillSafetyTime) && digitalRead(syringePin))
  {
    Serial.println( millis() - startTime );
  digitalWrite(led1,HIGH);    // turn on LED1
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led2,HIGH);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led3,HIGH);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led4,HIGH);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led5,HIGH);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led6,HIGH);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led1,LOW);    // turn on LED1
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led2,LOW);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led3,LOW);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led4,LOW);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led5,LOW);
  delay (ledPause);  // delay before next LED comes on
  digitalWrite(led6,LOW);
  delay (ledPause);  // delay before next LED comes on
  }
  digitalWrite(solenoidPin,LOW);   // pin  solenoid
  digitalWrite(buttonLed, LOW );
  Serial.println(" FULL");
}

void waitUnclip () {                  // wait for n seconds with lights still on before allowing next refill to start
}
//end utility functions
  
