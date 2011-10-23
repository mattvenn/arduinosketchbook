#include <TimedAction.h>

/*
nhs hull starter code
Matt Venn 2011 for Jam Jar Collective


  Hull NHS Colour-facade control code

  Developed by Adrian McEwen and Stuart Childs to monitor the
  light levels and outside temperature, and when it's dark light
  up different light colours based on the temperature

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
TimedAction updateLightsAction = TimedAction(10000,updateLights);

//defs from Adrian's code
bool gLightsOn = false;
const int kDuskThreshold = 250;
// Theshold at which we think it's getting light again
const int kDawnThreshold = 350;

// Number of seconds to wait between colour changes in the initial "it's gone dark, cycle through all possibilities" sequence
const unsigned long kInitialLightSequenceDelay = 5;

// Minimum value (NOT temperature) that the temperature sensor will read 
const int kMinTempValue = 600;
// Maximum value from analogRead that the temperature sensor will provide
const int kMaxTempValue = 700;


// Bits for the setLights number to color pattern mapping
const int kLightsOff = 0;
const int kRedBit = 1 << 0;
const int kGreenBit = 1 << 1;
const int kBlueBit = 1 << 2;
const int kYellowBit = 1 << 3;
const int kLightsOn = kRedBit | kGreenBit | kBlueBit | kYellowBit;

const int kTempIntervalCount = 15;
const int kTempMap[kTempIntervalCount] = {
 kRedBit,
 kYellowBit,
 kGreenBit,
 kBlueBit,
 kRedBit | kYellowBit,
 kRedBit | kGreenBit,
 kRedBit | kBlueBit,
 kYellowBit | kGreenBit,
 kYellowBit | kBlueBit,
 kGreenBit | kBlueBit,
 kRedBit | kYellowBit | kGreenBit,
 kRedBit | kYellowBit | kBlueBit,
 kRedBit | kGreenBit | kBlueBit,
 kYellowBit | kGreenBit | kBlueBit,
 kRedBit | kYellowBit | kGreenBit | kBlueBit
};



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
//  outputTestAction.check(); //turns all relay and LED channels on and off every 2 seconds
  updateElectricalAction.check(); //updates the data from the sensors
  statusAction.check(); //blinks the green light so we know everything is still running
  updateLightsAction.check();
}

void updateLights()
{

 if ((light < kDuskThreshold) && (gLightsOn == false))
 {
   // It's gone dark
   gLightsOn = true;

   // When it first goes dark, we cycle through all the light combinations
   for (int i = 0; i < 16; i++)
   {
     setLights(i);
     delay(1000UL * kInitialLightSequenceDelay);
   }
 }
 else if (light > kDawnThreshold)
 {
   // It's light again
   gLightsOn = false;
 }

 if (gLightsOn)
 {
   
   // Map this temperature value to  
   int tempValue = constrain(temp, kMinTempValue, kMaxTempValue);
   
//   setLights(kTempMap[map(tempValue, kMinTempValue, kMaxTempValue, 0, kTempIntervalCount)]);
  
  //ADDED BY STU - ATTEMPT AT MAPPING REVERSAL
  setLights(kTempMap[map(tempValue, kMaxTempValue, kMinTempValue, 0, kTempIntervalCount)]);

 }

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


// setLights
// Function to turn the lights on based on the provided aLightMask.
// aLightMask should be a number between 0 (all off) and 15 (all on)
// with each bit of that 4-bit number referring to a particular colour
// (see the kXxxBit constants to find out which bit is which colour)
void setLights(int aLightMask)
{
 //do this all the time
 digitalWrite(LEDS1, aLightMask & kRedBit);
 digitalWrite(LEDS2, aLightMask & kGreenBit);
 digitalWrite(LEDS3, aLightMask & kBlueBit);
 digitalWrite(LEDS4, aLightMask & kYellowBit);
 
 //only control main lights if it's dark
 if( gLightsOn )
 {
   digitalWrite(RELAY1, aLightMask & kRedBit);
   digitalWrite(RELAY2, aLightMask & kGreenBit);
   digitalWrite(RELAY3, aLightMask & kBlueBit);
   digitalWrite(RELAY4, aLightMask & kYellowBit);
 }
 else
 {
   digitalWrite(RELAY1, LOW );
   digitalWrite(RELAY2, LOW );
   digitalWrite(RELAY3, LOW );
   digitalWrite(RELAY4, LOW );
 }   
}
