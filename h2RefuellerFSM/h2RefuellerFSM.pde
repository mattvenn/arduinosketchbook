#include <FiniteStateMachine.h>

//Constants
#define  secondsRefuel 33  // total time to run electrolyser
#define  secondsUnclip 5  // after refuelling is complete, wait this long before entering Ready state for next refuel - allows time to unclip syringe
#define  BAUDRATE  115200  

//Pinouts
#define  button  12
#define  led1  4
#define  led2  5
#define  led3  6
#define  led4  7
#define  led5  8
#define  led6  9
#define  led7  10
#define  led8  11
#define  powerElectrolyser  13

//initialize states
State Ready = State(ledsOff);
State Refuelling = State(refuel);
State Waiting = State(waitUnclip);
double pauseTime;
FSM refuellerStateMachine = FSM(Ready);     //initialize state machine, start in state: Ready

void setup()  {
  pinMode (button, INPUT);      // 
  digitalWrite (button, HIGH);    // pulled up
  pinMode (led1, OUTPUT);
  pinMode (led2, OUTPUT);
  pinMode (led3, OUTPUT);
  pinMode (led4, OUTPUT);
  pinMode (led5, OUTPUT);
  pinMode (led6, OUTPUT);
  pinMode (led7, OUTPUT);
  pinMode (led8, OUTPUT);
  pinMode (powerElectrolyser, OUTPUT);
  digitalWrite (powerElectrolyser, LOW);  // pin for electrolyser off
  Serial.begin (BAUDRATE);
  Serial.print ("H2 REFUELLER    Time to fill: ");
  Serial.print (secondsRefuel);

  //force float eval with 1000.0
  pauseTime = (secondsRefuel*1000.0)/8;
  //8;
  //pauseTime = 4600;
  Serial.print( "pause time is " );
 
  Serial.print( pauseTime );
  
  Serial.print (" seconds    Wait for unclip: ");
  Serial.print (secondsUnclip);
  Serial.println (" seconds");
  
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
 digitalWrite(led7,LOW);
 digitalWrite(led8,LOW);
 

  Serial.println("Ready ");

 while(digitalRead(button)==HIGH) {    // wait for button to be pushed
   }
 }

void refuel () {
  Serial.print("Refuelling ");
  digitalWrite(powerElectrolyser,HIGH);   // pin for electrolyser on
  digitalWrite(led1,HIGH);    // turn on LED1
  Serial.print("1");          // serial print 1
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led2,HIGH);
  Serial.print("2");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led3,HIGH);
  Serial.print("3");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led4,HIGH);
  Serial.print("4");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led5,HIGH);
  Serial.print("5");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led6,HIGH);
  Serial.print("6");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led7,HIGH);
  Serial.print("7");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(led8,HIGH);
  Serial.print("8");
  delay (pauseTime);  // delay before next LED comes on
  digitalWrite(powerElectrolyser,LOW);   // pin  for electrolyser OFF
  Serial.println(" FULL");
}

void waitUnclip () {                  // wait for n seconds with lights still on before allowing next refill to start
  Serial.print("Waiting ");
  for(int i=1; i<secondsUnclip+1; i++) {
    Serial.print(i);
    delay (1000);
  }
  Serial.println("");
}
//end utility functions
  
