//#define testSteppers
//#define TEST_SD
#define testRadio
#define testLED
//#define testIO

#include <JeeLib.h>
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

#ifdef testSteppers

// #include <AccelStepper.h>
// AccelStepper stepperL(fsL, bsL); // use functions to step
// AccelStepper stepperR(fsR, bsR); // use functions to step


//  const int stepTime = 2;

#endif

MilliTimer sendTimer,statusTimer;


boolean ledState = false;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);   

  digitalWrite(led,HIGH);
  delay(2000);
  Serial.println("started");

  // initialize the digital pin as an output.
  pinMode(SERVO,OUTPUT);
  pinMode(MS1, OUTPUT );
  pinMode(MS2, OUTPUT );
  //pinMode( GPIO1, OUTPUT);
  //pinMode(GPIO2, OUTPUT );
  //  pinMode( GPIO1, INPUT );
  //  pinMode( GPIO2, INPUT );
  pinMode( SD_SEL, OUTPUT );
  pinMode( RFM_SEL, OUTPUT );

  pinMode( PWML, OUTPUT );
  pinMode( PWMR, OUTPUT );

  pinMode( DIRL, OUTPUT );
  pinMode( DIRR, OUTPUT );
  pinMode( STEPL, OUTPUT );
  pinMode( STEPR, OUTPUT );
  pinMode( LIMITL, INPUT );
  digitalWrite( LIMITL, HIGH );
  pinMode( LIMITR, INPUT );
  digitalWrite( LIMITR, HIGH );

  digitalWrite( SD_SEL, HIGH );
  digitalWrite(RFM_SEL, HIGH); 
  //spi test
  
  /*pinMode(SCK,OUTPUT);
  pinMode(MOSI,OUTPUT);

  pinMode(MISO,INPUT);
  digitalWrite(MISO,HIGH);
  pinMode(SS,OUTPUT);
  pinMode(RFM_INT,INPUT);
  digitalWrite(RFM_INT,HIGH);
  
  */
  // attachInterrupt(0, blink, RISING);

  
#ifdef testSteppers
  initSteppers();
#endif

#ifdef testRadio
  rf12_initialize(1, RF12_433MHZ,212);
  Serial.println( "rf12 setup done" );
#endif

}

volatile int b = 0;
void blink()
{
  b ++;
}

// the loop routine runs over and over again forever:
void loop() {

#ifdef testLED
  if( statusTimer.poll(500) )
  {
    Serial.println( "led");
    ledState = ! ledState;
    digitalWrite(led,ledState);
  }

#endif

#ifdef testSteppers
//  delay(100);
  moveSteppers();
#endif

#ifdef testServo 
  //lift servo
  pulsePower( 1, 20 );
  delay(2000);
  pulsePower( 1, 600 );
#endif

#ifdef testRadio
 // testRadioPins();
  doRadio();
#endif


#ifdef TEST_SD
  test_SD();
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




