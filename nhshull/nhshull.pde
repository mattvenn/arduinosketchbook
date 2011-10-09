#include <TimedAction.h>

/*
nhs hull starter code
*/

//pin defs
#define RELAY1 2
#define RELAY2 3
#define RELAY3 4
#define RELAY4 5

#define TEMP A0
#define LDR A1

#define LEDS1 A5
#define LEDS2 A4
#define LEDS3 A3
#define LEDS4 A2

#define LED_GREEN 13
#define LED_RED 12

#define LDR_MIN 0 
#define LDR_MAX 600
//globals
unsigned int light, temp;
boolean LED_OK = false;

TimedAction statusAction = TimedAction(500,blink);
TimedAction updateElectricalAction = TimedAction(500,updateElectrical);
void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(LED_GREEN, OUTPUT);     
  pinMode(LED_RED, OUTPUT);     
  pinMode(LED_GREEN, OUTPUT);   
  pinMode(LEDS1, OUTPUT);
  pinMode(LEDS2, OUTPUT);
  pinMode(LEDS3, OUTPUT);
  pinMode(LEDS4, OUTPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  Serial.begin(9600);
  Serial.println("nhs hull starting...");
}


void loop()
{
  //testAllOutputs();  //flashes all outs on and off
  updateElectricalAction.check();
  statusAction.check();
}

void blink()
{
  digitalWrite(LED_GREEN, LED_OK );
  LED_OK = ! LED_OK;
}
void updateElectrical()
{
  int lightRaw = analogRead( LDR );
  int tempRaw = analogRead( TEMP );
  
  //average to get rid of noise
  light = ( light * 0.5 ) + ( lightRaw * 0.5 );
  temp = ( temp * 0.5 ) + ( tempRaw * 0.5 );

  Serial.print( "light: " );
  Serial.println( light );
  Serial.print( "temp: " );
  Serial.println( temp ); 
  
  
  int lightsOn = map( light, LDR_MIN, LDR_MAX, 0, 3 );
  Serial.print( "light mapped to LED: " );
  Serial.println( lightsOn );
  
  //don't do this for real, only turn stuff off if we have to otherwise we'll get flicker.
  digitalWrite( LEDS1, LOW );
  digitalWrite( LEDS2, LOW );
  digitalWrite( LEDS3, LOW );
  digitalWrite( LEDS4, LOW );

  //only turns on leds, not the relays
  if (lightsOn >= 0 )
    digitalWrite(LEDS1, HIGH );
  if( lightsOn >= 1 )
    digitalWrite(LEDS2, HIGH );
  if( lightsOn >= 2 )
    digitalWrite(LEDS3, HIGH );
  if( lightsOn >= 3 )
    digitalWrite(LEDS4, HIGH );
    
 
}
void testAllOutputs() {
  

  digitalWrite(LED_GREEN, HIGH);   // set the LED on
  digitalWrite(LED_RED, HIGH);   // set the LED on

  delay(500);
  
  digitalWrite(LED_GREEN, LOW);   // set the LED on
  digitalWrite(LED_RED, LOW);   // set the LED on
   
  digitalWrite(LEDS1, HIGH);   // set the LED on
  digitalWrite(RELAY1, HIGH);   // set the LED on
  
  delay( 500 );

  digitalWrite(LEDS1, LOW);   // set the LED on
  digitalWrite(RELAY1, LOW);   // set the LED on
  
  digitalWrite(LEDS2, HIGH);   // set the LED on
  digitalWrite(RELAY2, HIGH);   // set the LED on

  delay(500);
  
  digitalWrite(LEDS2, LOW);   // set the LED on
  digitalWrite(RELAY2, LOW);   // set the LED on

  digitalWrite(RELAY3, HIGH);   // set the LED on 
  digitalWrite(LEDS3, HIGH);   // set the LED on
  
  delay(500);

  digitalWrite(LEDS3, LOW);   // set the LED on
  digitalWrite(RELAY3, LOW);   // set the LED on

  digitalWrite(LEDS4, HIGH);   // set the LED on
  digitalWrite(RELAY4, HIGH);   // set the LED on

  delay(500);              // wait for a second
  
  digitalWrite(LEDS4, LOW);   // set the LED on
  digitalWrite(RELAY4, LOW);   // set the LED on 
 
  
}


