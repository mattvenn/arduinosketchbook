#include "buttons.h"

void buttons_setup()
{
    pinMode(BUT_IN, INPUT);
    pinMode(BUT_OUT, INPUT);
    pinMode(BUT_HOME, INPUT);
    pinMode(BUT_LIMIT, INPUT);
    digitalWrite(BUT_IN, HIGH);
    digitalWrite(BUT_OUT, HIGH);
    digitalWrite(BUT_HOME, HIGH);
    digitalWrite(BUT_LIMIT, HIGH);
}

enum ButtonStatus buttons_check()
{
    if(digitalRead(BUT_LIMIT) == LOW)
        return(LIMIT);
    else if(digitalRead(BUT_IN) == LOW)
        return(IN);
    else if(digitalRead(BUT_OUT) == LOW)
        return(OUT);
    else if(digitalRead(BUT_HOME) == LOW)
        return(HOME);
    else
        return NONE;
}
