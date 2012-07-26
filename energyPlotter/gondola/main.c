/*
do I actually need a microcontroller to do this?
more example code:http://metku.net/index.html?path=articles/microcontroller-part-2/index_eng5
*/
#include "common.h"
const uint8_t startServoPos = 1;
//used for the 2 wire comms
uint8_t counter = 0; 
bool counting = false;

//pins
#define INT_PIN PB1
#define SERVO_PIN PB0 //OC0A
#define LED_PIN PB4

int main()
{

    //setup
	cli();
    //output setup
	setbit(DDRB,SERVO_PIN); //servo
    setbit(DDRB,LED_PIN); //led
    clearbit(DDRB,INT_PIN); //external interrupt

    //timer 0 setup:
    // Set prescaler to 64
    // 1 MHz / 64*256 = 61 Hz PWM frequency
	setbit(TCCR0B,CS01); //prescalar
	setbit(TCCR0B,CS00); //prescalar

	setbit(TIMSK0,TOIE0); //overflow interrupt enable
    setbit(TIMSK0,OCIE0A); //compare A interrupt enable

    // Fast PWM
    TCCR0A |= (1<<WGM01) | (1<<WGM00);

    // Clear OC0A/OC0B on Compare Match
    // Set OC0A/OC0B at BOTTOM (non-inverting mode)
    TCCR0A |= (1<<COM0A1);

    //load initial servo pos into compare reg
    OCR0A = startServoPos;

    //external int setup for comms
    setbit(MCUCR,ISC00); //rising edge of int0
    setbit(MCUCR,ISC01); //rising edge of int0
    setbit(GIMSK,INT0); //enable external int0 
    
    //flash led on powerup
    setbit(PORTB,LED_PIN); //turn on led
    _delay_ms(200);
    clearbit(PORTB,LED_PIN); //turn off led

    //enable interrupts
    sei();

    while(1)
    {
        //interrupts do all the work
    }
}

//int0 
ISR(INT0_vect)
{
    if( ! counting )
    {
        setbit(PORTB,LED_PIN); //led on
        counting = true;
        counter = 0;
    }
    else
    {
        clearbit(PORTB,LED_PIN); //led off
        counting = false;
        //load counter value into the timer compare reg
        OCR0A = counter;
    }
}

//timer 1 is used for servo waveform generation
ISR(TIM0_OVF_vect)
{
    //servo pin goes high
    //setbit(PORTB,SERVO_PIN);
    //comms counter
    if(counting)
        counter ++;
    //not sure if this is necessary, but it resets the counting stuff if it gets out of phase
    if( counter > 100 )
    {
      counting = false;
      counter = 0;
      clearbit( PORTB, LED_PIN );
    }
}

ISR(TIM0_COMPA_vect)
{
    //servo pin goes low
    //clearbit(PORTB,SERVO_PIN);
}

