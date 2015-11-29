#ifndef buttons_h
#define buttons_h

#include "Arduino.h"

#define HOME_PWM 50

enum ButtonStatus { IN, OUT, HOME, LIMIT, NONE };

#define BUT_IN A0
#define BUT_OUT A1
#define BUT_HOME A2
#define BUT_LIMIT A3

void buttons_setup();
enum ButtonStatus buttons_check();

#endif
