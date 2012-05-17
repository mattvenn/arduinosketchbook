/*
example program for running a fuel cell stack
for more info see the website:
http://www.arcolaenergy.com/h2mdk
*/

#include <h2mdk.h>

//pass V3W, V12W or V30W depending on your board and fuelcell
//for greater accuracy set second argument to arduino +5V in mV
h2mdk fuelcell(V3W,5000); 

void setup()
{
  Serial.begin(9600);
  //this blocks until caps are charged
  fuelcell.start();
}

void loop()
{
  //this takes care of short circuit, purging and updating electrical values
  fuelcell.poll();

  //you can get current and voltage from these functions:
  if( fuelcell.getCurrent() > 3 )
    Serial.println( "current too high" );
  if( fuelcell.getVoltage() < 1 )
    Serial.println( "voltage too low" );
}


