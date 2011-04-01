#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 48
#define STATUS 9
// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to

#include "WProgram.h"
void setup();
void loop();
Stepper stepper(STEPS, 5,6,7,8);

// the previous reading from the analog input
int previous = 0;

void setup()
{
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(200); //double speed because we're running at 8Mhz
  pinMode( STATUS, OUTPUT ); //status LED
  
    
}
int pos = 0;
void loop()
{
 

  // get the sensor value
  int val = map( analogRead(5), 0, 1024, 0, STEPS * 2);
  pos = pos * 0.5 + val * 0.5;
  // move a number of steps equal to the change in the
  // sensor reading
  if( pos != previous )
  {
    digitalWrite( STATUS, LOW );
    stepper.step(pos - previous);
    digitalWrite( STATUS, HIGH );
  }

  // remember the previous value of the sensor
 previous = pos;
  
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

