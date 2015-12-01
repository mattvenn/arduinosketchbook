#include "timer.h"

volatile byte timer2_overflows = 0;
uint8_t timer2_overflow = 0;
uint8_t timer2_remainder = 0;

ISR(TIMER2_OVF_vect)        
{
    timer2_overflows ++;
    if(timer2_overflows == timer2_overflow)
    {
        //preload timer
        TCNT2 = timer2_remainder;
    }
    if(timer2_overflows > timer2_overflow)
    {
        timer2_overflows = 0;
        calc = true;
        TCNT2 = 0;
    }
}

void setup_timer2()
{

    // timer 2 setup
    TCCR2A = 0;
    TCCR1B = 0;

    // set timer prescalar to 1024
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    // turn on interrupt overflow
    TIMSK2 |= (1 << TOIE2);

    // preload timer2 (8 bits)
    timer2_overflow = 1;
    timer2_remainder = 256 - 56;
    TCNT2 = 0;

    // set pwm frequency on pins 9&10 (timer1) to 31250Hz
    TCCR1B = TCCR1B & 0b11111000 | 0x01;
}

