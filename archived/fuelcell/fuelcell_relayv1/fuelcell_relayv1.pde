                                                                     
                                                                     
                                                                     
                                             
#define purgePin 4
#define shortStack1Pin 5
#define shortStack2Pin 6
#define stack2LedPin 7
#define stack1LedPin 8
#define systemLedPin 9
//#define stackVoltagePin A1
#define outputVoltagePin A0
#define stack1CurrentPin A4
#define stack2CurrentPin A3
#define outputCurrentPin A2



float purgeInterval = 10000;  // 240000ms = 4 mins
float purgeDuration =20;    // 100ms every 4 mins
float shortCircuitInterval = 5000;
float shortCircuitDuration =100;
float displayStatusInterval = 5000;
//float rawStackVoltage = 0;
float rawOutputVoltage = 0;
float rawStack1Current = 0;
float rawStack2Current = 0;
float rawOutputCurrent = 0;

//float stackVoltage = 0;
float outputVoltage = 0;
float stack1Current = 0;
float stack2Current = 0;
float outputCurrent = 0;


int startupDelay = 1;
int startupPurgeduration = 20;       // duration of purge on startup
boolean autoPurge = HIGH;
char inkey = ' ';


//float nextPurgeTime = purgeInterval;          // initialise with interval
float nextShortCircuitTime1 = 0;
float nextShortCircuitTime2 = 0;
float nextDisplayStatusTime = 0;
float nextPurgeTime = 0;

void setup() {                
  pinMode(purgePin, OUTPUT);     
  digitalWrite( purgePin, LOW);
  pinMode(shortStack1Pin, OUTPUT);    
  pinMode(shortStack2Pin, OUTPUT);  

  digitalWrite(shortStack1Pin, LOW); 
  digitalWrite(shortStack2Pin, LOW);   
  pinMode(systemLedPin, OUTPUT);     
  pinMode(stack1LedPin, OUTPUT);    
  pinMode(stack2LedPin, OUTPUT);
//  pinMode(stackVoltagePin, INPUT);
  pinMode(outputVoltagePin, INPUT);
  pinMode(stack1CurrentPin, INPUT);
  pinMode(stack2CurrentPin, INPUT);
  pinMode(outputCurrentPin, INPUT);


  Serial.begin (9600);
  Serial.println("");  
  Serial.println("RESET");


  digitalWrite(systemLedPin, HIGH);

  delay(5200);

  // startup purge
  displayInfoNoLF("STARTUP PURGE ");
  Serial.print(startupPurgeduration);

  Serial.println("ms ");

  purge(); //do a short one the same as the others, and store the purge time so we dno't do it twice unneccessarily
/*
  digitalWrite(purgePin, HIGH);
  delay(startupPurgeduration);
  digitalWrite(purgePin, LOW);
*/
  //startup shortcircuit
  delay(200);
  shortCircuit1 ();
  delay(200);
  shortCircuit2 ();
  nextShortCircuitTime1 = millis() + (shortCircuitInterval/2);
  nextShortCircuitTime2 = millis() + (shortCircuitInterval);
  displayInfo("SYSTEM RUNNING");
  Serial.println();
}


void loop() {

  if(millis() >= nextShortCircuitTime1) {
    shortCircuit1();
  }
  if(millis() >= nextShortCircuitTime2) {
    shortCircuit2();
  }

  if (millis() >= nextPurgeTime) {
    purge();

  }
  if(millis() >= nextDisplayStatusTime) {
    displayInfo("");
    nextDisplayStatusTime = millis()+displayStatusInterval;

  }


  if (Serial.available()>0) {    // only run this if there is something in serial buffer

    delay(20);  // had trouble here. wondered if there was a timing issue with reading in the chars from buffer , so added delay to time to get them
    inkey=Serial.read();
    if (inkey=='s') {                  // short circuit
      shortCircuit1();
      delay(1000);                    // To do: make control to separately short stack 1/stack 2
      shortCircuit2();
    }

    if (inkey=='p') {                  // purge circuit
      purge();
    }
    if (inkey=='e') {                  // purge valve stick open 
      purgeValveOpen();
      autoPurge = LOW;
    }
    if (inkey=='f') {                  // purge valve closed (back to auto)
      purgeValveClosed();
      autoPurge = HIGH;
    }
  }
}


void purge() {
  delay(400);
  nextPurgeTime=nextPurgeTime+purgeInterval;
  digitalWrite(systemLedPin, LOW);
  displayInfoNoLF("PURGE ");
  Serial.print(purgeDuration, 0);
  Serial.println("ms");
  digitalWrite(purgePin, HIGH); 
  delay(purgeDuration);           
  digitalWrite(purgePin, LOW);  
  digitalWrite(systemLedPin, HIGH);
  delay(400);
}

void shortCircuit1() {
  nextShortCircuitTime1=nextShortCircuitTime1+(shortCircuitInterval);
  displayInfoNoLF("SHORT CIRCUIT Stack 1  ");
  Serial.print(shortCircuitDuration, 0);
  Serial.println("ms");
  digitalWrite(shortStack1Pin, HIGH); 
  delay(shortCircuitDuration);           
  digitalWrite(shortStack1Pin, LOW); 
 
 //separate led and relay
  digitalWrite(stack1LedPin, HIGH);
  delay(shortCircuitDuration);           
  digitalWrite(stack1LedPin, LOW);
}

void shortCircuit2() {
  nextShortCircuitTime2=nextShortCircuitTime2+(shortCircuitInterval);
  displayInfoNoLF("SHORT CIRCUIT Stack 2  ");
  Serial.print(shortCircuitDuration, 0);
  Serial.println("ms");
  digitalWrite(shortStack2Pin, HIGH); 

  delay(shortCircuitDuration);           
  digitalWrite(shortStack2Pin, LOW); 

 //separate led and relay
  digitalWrite(stack2LedPin, HIGH);
  delay(shortCircuitDuration);           
  digitalWrite(stack2LedPin, LOW);
}


void purgeValveOpen() {
  digitalWrite(purgePin, HIGH); 
  displayInfo("PURGE VALVE OPEN");
}
void purgeValveClosed() {
  digitalWrite(purgePin, LOW); 
  displayInfo("PURGE VALVE CLOSED");
}

void displayTime() {
  Serial.print((millis()/1000));
  Serial.print("    ");
}

void delaySeconds(int j) {
  for (int k=0; k<j; k++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
}


void displayInfo (String str) {
  displayTime();
  displaySensors();  
  Serial.println(str);
}

void displayInfoNoLF (String str) {
  displayTime();
  displaySensors();
  Serial.print(str);
}

void displaySensors () {
  readSensors();
  //    Serial.print(rawCurrent, 0);
  //    Serial.print(" ");    
  Serial.print(outputVoltage);
  Serial.print("V  ");

  Serial.print(rawStack1Current);
  Serial.print("  ");
  Serial.print(stack1Current);
  Serial.print("A  ");
  Serial.print(rawStack2Current);
  Serial.print("  ");
  Serial.print(stack2Current);
  Serial.print("A  ");

  Serial.print(outputCurrent);
  Serial.print("A  ");
  //    Serial.print(rawTemp, 0);
  //    Serial.print(" ");    

}

void readSensors () {
  rawOutputVoltage = analogRead(outputVoltagePin);
  rawStack1Current = analogRead(stack1CurrentPin);
  rawStack2Current = analogRead(stack2CurrentPin);
  rawOutputCurrent = analogRead(outputCurrentPin);

  outputVoltage = ((rawOutputVoltage*4.92/1024));
  stack1Current = ((rawStack1Current-512.00)/37);
  stack2Current = ((rawStack2Current-512.00)/37);

}

