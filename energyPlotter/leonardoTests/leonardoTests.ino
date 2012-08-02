/* 
total program size is 19k with sd and radio, 9k without sd
grbl is about 17k. Available is 28k with bootloader.
*/
#define testSteppers
#define useSD //uses 10k
#define useRadio //uses 180bytes?!
#define testLED
#define testServo
//#define testIO
//#define testMem


#ifdef useRadio
#include <JeeLib.h>
MilliTimer statusTimer;
#endif

#include <Stepper.h>

//pin defs
#define DIRR 0
#define DIRL 4

#define LIMITL 13
#define LIMITR 5
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
int servoPos = 20;
boolean ledState = false;
boolean testSD = false;
boolean checkRadio = false;

//payload def
typedef struct {
  char command;
  unsigned int arg1;
  unsigned int arg2;

} Payload;
Payload payload;

void setup() {
  Serial.begin(9600);
  Serial.println("started");
  
  pinMode(led, OUTPUT);   
  digitalWrite(led,HIGH);

  // initialize the digital pin as an output.
  pinMode(SERVO,OUTPUT);
  setPowerPin(LOW);

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
  //  pinMode( GPIO1, INPUT );
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

  //leave some time in case this doesn't work. Makes it easier to reprogram!  
  delay(4000);
  #ifdef useSD
    initSD();
  #endif
  #ifdef useRadio
  initRadio();
  checkRadio = true;    
  #endif
  
}

// the loop routine runs over and over again forever:
void loop() {


  if( statusTimer.poll(500) )
  {
    #ifdef testLED
    Serial.println( "led");
    ledState = ! ledState;
    digitalWrite(led,ledState);
    #endif
    #ifdef testMem
    Serial.print("mem:");
    Serial.println(freeMemory());
    #endif

  }

 /* if( testSD && sdTimer.poll(5000) )
  {

   // readSD();
    writeSD(i++);
    //work out why we need this?
    initRadio();

  }
*/

  if(Serial.available() > 0 )
  {
    payload.command = Serial.read();
    payload.arg1 = serReadInt();
    payload.arg2 = serReadInt();
    commandWaiting = true;
    Serial.flush();
  }

  if( commandWaiting )
  {
    commandWaiting = false;
    switch( payload.command )
    {
      case 's':
        pulsePower( 1, payload.arg1 );
      break;
      case 'm':
        moveSteppers();
        break;
      case 'p':
        setSpeed(payload.arg1);
      break;
      #ifdef useSD
      case 'w':
        writeSD(payload.arg1);
        readSD();
        #ifdef useRadio
        initRadio(); //need this after a write/read some combo?
        #endif
        break;  
      case 't':
        testSD = payload.arg1;
        Serial.println( testSD );
        break;
      #endif
      #ifdef useRadio
      case 'r':
        initRadio();
        checkRadio = true;
        break;
      #endif
      default:
        Serial.println( "bad command");
        break;
    }
     sendAck = true;   
  }
#ifdef useRadio        
if( checkRadio)
  doRadio();
#endif

#ifdef testIO
  Serial.print( "ints: " );
  Serial.println( b );
  Serial.print( "gpio1:" );

  Serial.println( analogRead( GPIO1_IN ) );
  Serial.print( "gpio2:" );
  Serial.println( analogRead( GPIO2_IN ) );
  myServo.write( 0 );
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(MS1, HIGH );
  digitalWrite(MS2, HIGH );
  //  digitalWrite( GPIO1, HIGH);
  //  digitalWrite(GPIO2, HIGH );
  digitalWrite( SD_SEL, HIGH );
  digitalWrite( RFM_SEL, HIGH );

  digitalWrite( STEPL, HIGH );
  digitalWrite( STEPR, HIGH );
  digitalWrite( DIRL, HIGH );
  digitalWrite( DIRR, HIGH );

  Serial.println( digitalRead(LIMITL) ? "LIMITL 1" : "LIMITL 0" );
  Serial.println( digitalRead(LIMITR) ? "LIMITR 1" : "LIMITR 0" );  
  analogWrite(PWML, 100 );
  analogWrite(PWMR, 100 );

  delay(1000);       

  digitalWrite( STEPL, LOW );
  digitalWrite( STEPR, LOW );
  digitalWrite( DIRL, LOW );
  digitalWrite( DIRR, LOW );

  // wait for a second
  digitalWrite(led, LOW);
  digitalWrite(MS1, LOW );
  digitalWrite(MS2, LOW );
  //  digitalWrite( GPIO1, LOW);
  //  digitalWrite(GPIO2, LOW );
  digitalWrite( SD_SEL, LOW );
  digitalWrite( RFM_SEL, LOW );
  analogWrite(PWML, 10 );
  analogWrite(PWMR, 10 );
  // turn the LED off by making the voltage LOW

#endif
}
