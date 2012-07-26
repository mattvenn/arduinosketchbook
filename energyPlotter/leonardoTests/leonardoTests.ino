#include <Servo.h>
#define SERVO A4


#define DIRR 0
#define LIMITL 1
#define STEPR 2
#define LIMITR 4
#define STEPL 5
#define GPIO1 6
#define led 7
#define MS1 8
#define MS2 9
#define PWML 10
#define PWMR 11

#define GPIO2 12

#define DIRL 13

#define GPIO1_IN A7
#define GPIO2_IN A11
#define SD_SEL A3
#define RFM_SEL A5

#define RFM_INT 3

Servo myServo;
// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  Serial.println("started");
  myServo.attach(SERVO);  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);   
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
  
 attachInterrupt(0, blink, RISING);
}

volatile int b = 0;
void blink()
{
  b ++;
}
// the loop routine runs over and over again forever:
void loop() {
  
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


  myServo.write( 180 );
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
  delay(1000);               // wait for a second
}
