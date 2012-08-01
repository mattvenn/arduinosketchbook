/*
do I actually need a microcontroller to do this?
more example code:http://metku.net/index.html?path=articles/microcontroller-part-2/index_eng5
*/
#include "common.h"
//#include <avr/eeprom.h>

//uint8_t  EEMEM startServoLoc; 
//uint8_t startServoPos;
//used for the 2 wire comms
volatile uint8_t counter = 0; 
volatile bool counting = true;

//pins
#define INT_PIN PB1
#define SERVO_PIN PB0 //OC0A
#define LED_PIN PB4

int main()
{

    //setup
	cli();

  //read last servo pos:
 // startServoPos = eeprom_read_byte(&startServoLoc); 
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


    //load initial servo pos into compare reg
//    OCR0A = startServoPos;

    //external int setup for comms
    setbit(MCUCR,ISC00); //rising edge of int0
    setbit(MCUCR,ISC01); //rising edge of int0
    setbit(GIMSK,INT0); //enable external int0 
    
    //flash led on powerup
    setbit(PORTB,LED_PIN); //turn on led

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
    cli();
    if( counting )
    {
        clearbit(PORTB,LED_PIN); //led off
        counting = false;
        //load counter value into the timer compare reg
        OCR0A = counter;
        //save servo pos to eeprom
        //eeprom_write_byte(&startServoLoc,counter);

        //turn on PWM servo output
        // Clear OC0A/OC0B Pin on Compare Match
        // Set OC0A/OC0B at BOTTOM (non-inverting mode)
        TCCR0A |= (1<<COM0A1);
    }
    sei();
}

//timer 1 is used for servo waveform generation
ISR(TIM0_OVF_vect)
{
    //servo pin goes high
    //setbit(PORTB,SERVO_PIN);
    //comms counter
    if(counting)
        counter ++;
}

ISR(TIM0_COMPA_vect)
{
    //servo pin goes low
    //clearbit(PORTB,SERVO_PIN);
}

