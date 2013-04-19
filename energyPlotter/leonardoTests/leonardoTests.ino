/* 
 total program size is 19k with sd and radio, 9k without sd
 grbl is about 17k. Available is 28k with bootloader.
 
 strange spi problems where radio stopped sending after an sd write has stopped.
 Just discovered this is because the spi programmer was still plugged in. 
 Doesn't need to be on, just attached. 
 So must be an electrical characteristic of the spi bus is wrong 
 eg pullup/down resistors required. a pulldown of 1k on miso seems to solve the problem.
 setting miso to pullup prevents radio from working but sd still works.
 check with a scope
 
 jeelib/rf12.cpp needs adjusting to set rf12 chip select to portf bit 0
 
 todo:
 sw:
 - check: after calibration, with strings equal length, x and y aren't quite right I think. problem with FK?
 + update all config values so they're in mm. convert to steps internally
 + not doing. the limits returned by the u command should be slightly bigger than the actual robot's limits

 hw:
 - replace stepper driver with stepsticks?
 - if gpio doesn't include i2c then probably it should
 
 */
#define testSteppers
#define useSD //uses 10k
//#define useRadio //uses 180bytes?!
#define testLED
#define testServo
//#define testIO
//#define testMem
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "config.h"
#include <SPI.h>
#include <JeeLib.h>
#include "datatype.h"
MilliTimer statusTimer,sdTimer;

//#include <AccelStepper.h>

//servo constants
#define PENDOWN 300
#define PENUP 450
#define PULSELEN 2

//pin defs
#define DIRR 0
#define DIRL 4

#define LIMITL 5
#define LIMITR 13 
#define SERVO A3
#define STEPR 2
#define STEPL 1
#define GPIO1 6
#define led 7
#define MS1 8
#define MS2 9
#define PWML 10
#define PWMR 11

#define GPIO2 12

#define GPIO1_IN A7
#define GPIO2_IN A11
#define SD_SEL A4
#define RFM_SEL A5
#define RFM_INT 3

boolean commandWaiting = false;
boolean sendAck = false;
boolean ledState = false;
boolean servoState = false;
boolean testSD = false; //auto test the SD
boolean checkRadio = false;
boolean servoTest = false;
int i = 0;
long int lastCommandTime = 0;
boolean calibrated = false;

//all of this stored in eeprom


long commandsExecuted = 0;
int x1; //measured in steps
int y1; //measured in steps
long int a1; //measured in steps
long int b1; //measured in steps


//pwm power stuff
boolean lowpower = false;
const int low_power_time = 2000; //pwm low after 2 seconds of not doing anything.

Payload payload;

boolean storeCommands = false;
unsigned int expectedCommands = 0;
unsigned int storedCommands = 0;
boolean penState = 0;

void setup() {
  //load our config from eeprom
  loadConfig();
  Serial.begin(57600);
  //  EEPROM.write(idAddress,1); //set address
  //delay(5000);
  Serial.print("started, robot id:");
  Serial.println(config.id);
  pinMode(led, OUTPUT);   
  digitalWrite(led,HIGH);

  // initialize the digital pin as an output.
  pinMode(SERVO,OUTPUT);

  //stepper microstep control pins  
  pinMode(MS1, OUTPUT );
  pinMode(MS2, OUTPUT );
  pinMode( PWML, OUTPUT );
  pinMode( PWMR, OUTPUT );
  //stepper step and dir
  pinMode( DIRL, OUTPUT );
  pinMode( DIRR, OUTPUT );
  pinMode( STEPL, OUTPUT );
  pinMode( STEPR, OUTPUT );

  //pinMode( GPIO1, OUTPUT);
  //pinMode(GPIO2, OUTPUT );
  pinMode( GPIO1, INPUT );
  digitalWrite(GPIO1,HIGH);
  //  pinMode( GPIO2, INPUT );

  //spi setup
  pinMode( SS, OUTPUT ); //needed to make us the master of spi bus
  pinMode( SD_SEL, OUTPUT );
  pinMode( RFM_SEL, OUTPUT );
  digitalWrite( SD_SEL, HIGH );
  digitalWrite(RFM_SEL, HIGH); 

  //limits
  pinMode( LIMITL, INPUT );
  digitalWrite( LIMITL, HIGH );
  pinMode( LIMITR, INPUT );
  digitalWrite( LIMITR, HIGH );

  //config steppers
  initSteppers();
  initServo();
  //leave some time in case this doesn't work. Makes it easier to reprogram!  
  //delay(4000);

#ifdef useRadio
  initRadio();
  checkRadio = true;    
#endif
  //put init sd after radio to make the spi speed change stick
#ifdef useSD
  initSD();
#endif
  //calibrate();
}

// the loop routine runs over and over again forever:
void loop() {

  if(digitalRead(GPIO1)==LOW)
    home();
  if( statusTimer.poll(500) )
  {

#ifdef testLED
    //Serial.println( "led");
    ledState = ! ledState;
    digitalWrite(led,ledState);
#endif
#ifdef testMem
    Serial.print("mem:");
    Serial.println(freeMemory());
#endif
    //try to cope with lost packets. Send an ack if enough time has elapsed since we last completed a command.
    /*     if( millis() - lastCommandTime > 10000 ) 
     {
     sendAck = 1;
     Serial.println( millis() );
     }*/
  }

  if(Serial.available() > 0 )
  {
    payload.command = Serial.read();
    payload.arg1 = serReadFloat();
    payload.arg2 = serReadFloat();
    commandWaiting = true;
    Serial.flush();
  }

  if( commandWaiting )
  {
    commandWaiting = false;

    //   stopRadio();

    if(storeCommands)
    {
      if(payload.command == 's')
      {
        storeCommands = false;
        Serial.println("finished storing");
        printStoredCommands(false);
        Serial.println(storedCommands-expectedCommands);
        Serial.println("ok");
        expectedCommands = 0;

      }
      else
      {
        storeCommand();
        Serial.print("storing command:");
        Serial.println(storedCommands++);
        Serial.println("ok");
      }
    }
    else
    {
      runCommand(&payload);
    }
    //    startRadio();
    //    initRadio();
    sendAck = true;   
    lastCommandTime = millis();
    //  Serial.print( "command finished at: " ); // putting these in will break the feeder 
    // Serial.println( lastCommandTime ); //putting these in will break the feeder
  }
#ifdef useRadio        
  if( checkRadio)
    doRadio();
#endif

  if(millis() - lastCommandTime > low_power_time && lowpower == false)
    powerSave(true);

}

void runCommand( Payload * p)
{
  if(!calibrated && !(p->command == 'c'||p->command == 'f'))
  {
    Serial.println("not calibrated");
    Serial.println("ok");
    return;
  }
  //  Serial.println("run command called:");
  //  printPayload(p);
  switch( p->command )
  {
  case 'b':
    calibrate(p->arg1);
    break;
  case 'c':
    home();
    Serial.println("ok");
    break;
  case 'd':
    // if( p->arg1 != penState )
    {
      Serial.println( p->arg1 ? "pen down" : "pen up" );
      p->arg1 ? penDown() : penUp();

      Serial.println( "ok");
      penState = p->arg1;
    }  
    /* else
     {
     Serial.print( "pen already: ");
     Serial.println( penState ? "down" : "up" );
     }*/
    break;
  case 'e': //execute stored commands
    printStoredCommands(true);
    Serial.println("ok");
    break;
  case 'f':
    a1 = p->arg1 * config.stepsPerMM;
    b1 = p->arg2 * config.stepsPerMM;
    Serial.println("a1 and b1 updated");
    FK(a1,b1);
    calibrated=true;
    Serial.println("ok");
    break;
  case 'g':
    drawLine(p->arg1*config.stepsPerMM,p->arg2*config.stepsPerMM);
    Serial.println( "ok" );
    break;
  case 'h':
    printStoredCommands(false);
    Serial.println("ok");
    break;
  case 'i':
    setMS(p->arg1);
    Serial.println("ok");
    break;
  case 'j':
    dumpSerialConfig();
    Serial.println("ok");
    break;
  case 'k':
    //weird case, we need to send more data than usual, so we return an ok response for the feed program, then wait for more data in loadSerialConfig
    Serial.println("ok");
    loadSerialConfig();
    Serial.println("ok");
    break;
  case 'm':
    stepLeft(p->arg1);
    stepRight(p->arg2);
    Serial.println("ok");
    break;
  case 'p':
    setSpeed(p->arg1);
    setPWM(p->arg2);
    config.default_pwm = p->arg2;
    config.draw_speed = p->arg1;
    Serial.println("ok");
    break;
  case 'q':
    //rectangular coords
    Serial.print( "x: ");
    Serial.println(x1 / config.stepsPerMM);

    Serial.print( "y: ");
    Serial.println(y1 / config.stepsPerMM);

    //string lengths
    Serial.print( "a1 (no hanger): ");
    Serial.println(a1 / config.stepsPerMM - config.hanger_l);

    Serial.print( "b1 (no hanger): ");
    Serial.println(b1 / config.stepsPerMM - config.hanger_l);

    //pen status
    Serial.println( penState ? "pen: down" : "pen: up" );
    Serial.print( "cmds exectuted: ");
    Serial.println(commandsExecuted);
    p->arg1 = x1 / config.stepsPerMM;
    p->arg2 = y1 / config.stepsPerMM;
    Serial.println("ok");
    break;
  case 'r':
    //initRadio();
    Serial.println( "useradio" );
    //   readSD();
    break;
  case 's': //store commands for later
    storeCommands = true;
    storedCommands=0;
    expectedCommands = p->arg1;
    Serial.println("storing subsequent commands");
    Serial.println("ok");
    break;
  case 't':
    servoTest = p->arg1; 
    Serial.print( "line tset: " ); 
    Serial.println( servoTest );
    break;
  case 'u':
    if( p->arg1 > 0)
      config.stepsPerMM = p->arg1;
    Serial.print("stepsPerMM: ");
    Serial.println(config.stepsPerMM);

    Serial.print("w: ");
    Serial.println(config.width);

    Serial.print("h: ");
    Serial.println(config.height);

    Serial.print("top margin: ");
    Serial.println(config.top_margin);
    Serial.print("side margin: ");
    Serial.println(config.side_margin);

    Serial.println("ok");
    break;
  case 'w':
    writeSD(p->arg1);
    readSD();
    //#ifdef useRadio
    //initRadio(); //need this after a write/read some combo?
    //#endif
    break;  
  case 'x':
    detachInterrupt(0);
    Serial.println( "detach inter" );
    SPI.end();
    break;
  case 'y':
    digitalWrite( SERVO, p->arg1 ? PENDOWN : PENUP );
    Serial.println("ok");
    break;   
  case 'z':
    initSD();
    break;
  default:
    Serial.println( "bad command");
    break;
  }
  commandsExecuted ++;
}

