//setup for stepper motor (our motor is 3.6degree)
#define STEPS 200
#include <Stepper.h>
Stepper stepper(STEPS, 2,3,4,5);
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
//led
#define LED 13
#define DC_PIN 6
//define solenoid pins
#define SOLENOID1 A0
#define SOLENOID2 A1
#define SOLENOID3 A2
#define SOLENOID4 A3
#define CLUTCH_SOLENOID A4

#define OPTO 9
int offset = 0;
volatile int rotor_count = 0;
boolean engaged = false;
int drivespeed = 30;
int pickupspeed = 10;
int pickupsteps = STEPS / 4;


void setup()
{
  Serial.begin(9600);
  //  Serial.println( "started" );
  // set the speed of the motor (RPMs)
  stepper.setSpeed(40); //40 works

  //pin modes of the control pins
  pinMode(LED,OUTPUT);
  pinMode(SOLENOID1,OUTPUT);
  pinMode(SOLENOID2,OUTPUT);
  pinMode(SOLENOID3,OUTPUT);
  pinMode(SOLENOID4,OUTPUT);
  pinMode(CLUTCH_SOLENOID,OUTPUT);
  pinMode(DC_PIN,OUTPUT);
  
  pinMode(OPTO,INPUT);
  PCintPort::attachInterrupt(OPTO, rotorCount,RISING);
  // clutch(5,180);
 // stepper.step(2000);

 testAll();
 
   
}


void testAll()
{
 //slider solenoids 
 for (int i = 0; i < 4; i ++ )
 {
   digitalWrite(A0+i,HIGH);
   delay(500);
   digitalWrite(A0+i,LOW);
   delay(500);
 }
 //stepper
 stepper.step(200);
 
 //opto
 Serial.println("got ");
 Serial.println(rotor_count);
 for(int i = 0; i < rotor_count; i ++ );
 {
    flash(LED,500);   
 }
 rotor_count = 0;
 
 //stepper clutch
 digitalWrite(CLUTCH_SOLENOID, HIGH);
 delay(500);
 digitalWrite(CLUTCH_SOLENOID, LOW);


  //dc motor
  digitalWrite(DC_PIN,HIGH);
  delay(500);
  digitalWrite(DC_PIN,LOW);
}

//opto interrupt routine
void rotorCount()
{
  rotor_count ++;
  Serial.println("int");
}


//keep doing this
void loop()
{
  if( Serial.available() )
  {
    char c = Serial.read();
    switch( c )
    {
    case 'd': //dc motor
      driveDC(serReadInt());
      break;
    case 'r': //reset
      reset_clips();
      Serial.println("ok");
      break;
    case 's': //set
      clutch(serReadInt(),180);
      break;
    case 'f': //rotate forward
      delay(100);
      stepper.step(serReadInt());
      Serial.println("ok");
      break;        
    case 'o': //set offset
      offset = serReadInt();
      Serial.print("set offset to ");
      Serial.println(offset);
      Serial.println("ok");
      break;
    case 'm': //set motor speed
      stepper.setSpeed(serReadInt());
      Serial.println("set stepper speed");
      Serial.println("ok");
      break;
    default:
      Serial.println("bad command");
      break;
    }
  }
}

//dc motor with opto test
void driveDC(int steps)
{
  //drives the dc motor till the opto count == the steps
  rotor_count = 0;
  digitalWrite(DC_PIN, HIGH);
  Serial.print("moving "); Serial.println(steps);
  while( rotor_count <= steps )
  {
    ;; //do nothing
  }
  digitalWrite(DC_PIN, LOW);
}
  
//for controlling the solenoids
void set(int sol1, int sol2, int sol3)
{
  Serial.println("setting solenoids to: ");
  Serial.print(sol1); 
  Serial.print(","); 
  Serial.print(sol2); 
  Serial.print(","); 
  Serial.println(sol3);

  sol1 = map(sol1, 0, 8, 0, STEPS);
  sol2 = map(sol2, 0, 8, 0, STEPS);
  sol3 = map(sol3, 0, 8, 0, STEPS);

  Serial.print(sol1); 
  Serial.print(","); 
  Serial.print(sol2); 
  Serial.print(","); 
  Serial.println(sol3);
  //during a whole turn, fire the solenoids on their random trigger point

  for( int stepNum =0; stepNum < STEPS; stepNum ++ )
  {

    //check if we need to turn on the solenoid
    if( stepNum + offset >= sol1 )
      digitalWrite(SOLENOID1,HIGH);
    if( stepNum + offset >= sol2 )
      digitalWrite(SOLENOID2,HIGH);
    if( stepNum + offset >= sol3 )
      digitalWrite(SOLENOID3,HIGH);


    //take one step on the motor
    stepper.step(1);
  }

  //turn off the solenoids
  digitalWrite(SOLENOID1,LOW);
  digitalWrite(SOLENOID2,LOW);
  digitalWrite(SOLENOID3,LOW);
}

//rotate backwards to reset all the rotors
void reset_clips()
{
  //turn motor back
  stepper.step(-100);
  stepper.step(8);
}

//do a wiggle, helps dog clutch disconnect
void wiggle(int steps,int loops)
{
  for( int i = 0; i < loops; i ++)
  {
    stepper.step(steps);
    stepper.step(-steps);
  }
}

//dog clutch tests
void clutch(int loops,int steps)
{
  float passes = 0;
  for( int i = 0; i < loops; i ++ )
  {
    int forwardcounts = 0;

    digitalWrite(SOLENOID1,HIGH);
    stepper.setSpeed(pickupspeed);
    stepper.step(pickupsteps);
    stepper.setSpeed(drivespeed);
    stepper.step(steps-pickupsteps);

    digitalWrite(SOLENOID1,LOW);
    wiggle(4,4);
    forwardcounts = rotor_count;
    rotor_count = 0;
    delay(1000);

    stepper.step(-steps);
    delay(1000);

    Serial.print( "loop: "); 
    Serial.print(i); 
    Serial.print(" forwards:" );
    Serial.print(forwardcounts);
    Serial.print(" backwards:");
    Serial.print(rotor_count);
    if( forwardcounts < 8 || rotor_count > 3 )
    {
      Serial.println(" FAIL");
    }
    else
    {
      Serial.println(" PASS");
      passes ++;
    }
    rotor_count = 0;
  }
  Serial.print( passes / loops * 100 );
  Serial.println("%");
}


//flash led
void flash(int pin, int delayT)
{
  digitalWrite(pin,HIGH);
  delay(delayT);
  digitalWrite(pin,LOW);
    delay(delayT);
}




