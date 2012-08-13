/*
example program for running a fuel cell stack
for more info see the website:
http://www.arcolaenergy.com/h2mdk
*/

//define _version as V1_5W, V3W, V12W or V30W depending on your board and fuelcell
#define _version V12W
#define _hardware V1
#include "h2mdk.h"
#include <JeeLib.h>

#define lowVoltageV 7.5
boolean voltageLow = false;
boolean blueState = false;

MilliTimer flasherTimer;

#define GREEN 9
#define BLUE 8
h2mdk fuelcell;

void setup()
{
  Serial.begin(9600);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);

  led(GREEN,false);
  //blue light on till poll finishes
  led(BLUE,true);

  //this blocks until caps are charged
  fuelcell.start();
  //short circuit time of 100ms stopped motors for 3secs. 70ms results in a brief falter.
  fuelcell.overrideTimings(10000,70,25000,50);
}

inline void led(int colour,bool state)
{
  digitalWrite(colour,!state);
}

void loop()
{
  //this takes care of short circuit, purging and updating electrical values
  fuelcell.poll();

  if( flasherTimer.poll(250) )
  {
    if( voltageLow )
    {
    //Serial.println( "v low" );
    led(BLUE,blueState);
    blueState = ! blueState;
    }
    else
    {
    //Serial.println( "v ok" );
    }
  }

  if( fuelcell.getVoltage() < lowVoltageV )
  {
    voltageLow = true;
    led(GREEN,false);
  }
  else
  {
    voltageLow = false;
    led(BLUE,false);
    led(GREEN,true);
  }
}
