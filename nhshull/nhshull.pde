#include <TimedAction.h>

/*
nhs hull starter code
Matt Venn 2011 for Jam Jar Collective

To log output in Linux bash shell:

stty -F /dev/ttyUSB0 9600 raw         #this sets up serial port /dev/ttyUSB0 (might need to be ttyUSB1)
cat /dev/ttyUSB0 | tee -a ~/nhs.output   #read the input, pipe it to a file in your home directory and also to the screen
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

boolean testCycle = false;

TimedAction statusAction = TimedAction(500,blink);
TimedAction updateElectricalAction = TimedAction(500,updateElectrical);
TimedAction outputTestAction = TimedAction(2000,testAllOutputs);

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
  outputTestAction.check(); //turns all relay and LED channels on and off every 2 seconds
  updateElectricalAction.check(); //updates the data from the sensors
  statusAction.check(); //blinks the green light so we know everything is still running
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
  
/*  
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
  */  
 
}
void testAllOutputs() {
  
  //invert state
  testCycle = ! testCycle;
 
  digitalWrite(LEDS1, testCycle);   
  digitalWrite(RELAY1, testCycle);   
  
  digitalWrite(LEDS2, testCycle);  
  digitalWrite(RELAY2, testCycle); 

  digitalWrite(RELAY3, testCycle);  
  digitalWrite(LEDS3, testCycle); 
  
  digitalWrite(LEDS4, testCycle);   
  digitalWrite(RELAY4, testCycle); 
  
}


