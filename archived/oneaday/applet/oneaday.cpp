#include <TimerOne.h>

#include "WProgram.h"
void setup();
void incrementTimer();
void loop();
int switchPin =  0;    // LED connected to digital pin 13
int LED1Pin = 1;
int LED2Pin = 2;
long unsigned int seconds = 0;
long unsigned int secondsInADay = 60 * 60 * 24;
boolean alarmState = true;

void setup()   {                
  //calibrate the internal 8mhz oscillator
  OSCCAL = 102;
  // initialize the io pins
  pinMode(switchPin, INPUT );     
  digitalWrite( switchPin, HIGH );
  pinMode(LED1Pin, OUTPUT );
  pinMode(LED2Pin, OUTPUT );
  //setup timer1
  Timer1.initialize(500000);
  Timer1.attachInterrupt(incrementTimer );
}


void incrementTimer()
{
  seconds ++; 
  //when 24 hours is up, then
  if( seconds >= secondsInADay )
  {
    seconds = 0;
    alarmState = true;
  }
}

void loop()                     
{
  // when someone presses the switch
  if( digitalRead( switchPin ) == LOW )
  {
    digitalWrite( LED1Pin, LOW );
    digitalWrite( LED2Pin, LOW );      
    alarmState = false;
  }
  
  //when 1 hour is up, then
  //superAlarmState = HIGH
  
  if( alarmState == true )
  {
     digitalWrite( LED1Pin, HIGH );
     digitalWrite( LED2Pin, HIGH );      
  }
}  

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

