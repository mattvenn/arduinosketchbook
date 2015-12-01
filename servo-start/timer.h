#ifndef timer_h
#define timer_h

extern volatile bool calc;

#include "Arduino.h"

//interrupt service routine 
ISR(TIMER2_OVF_vect);
void setup_timer2();

#endif
