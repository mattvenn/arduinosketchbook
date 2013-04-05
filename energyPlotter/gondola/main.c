/*
todo:

move to an always on mode. needs to have a listening mode and a moving mode. After timing out the moving mode, go back to listening again. 
also, don't accept timing info from master, use hardcoded numbers for moving the servo

more example code:http://metku.net/index.html?path=articles/microcontroller-part-2/index_eng5
*/
#include "common.h"
//#include <avr/eeprom.h>

#include <util/delay.h>

//uint8_t  EEMEM startServoLoc; 
//uint8_t startServoPos;
//used for the 2 wire comms
volatile uint8_t counter = 0; 
volatile bool counting = false;
volatile bool moveServo = false;
//pins
#define INT_PIN PB3
#define SERVO_PIN PB0 //OC0A
#define LED_PIN PB4

#define PENUP 16
#define PENDOWN 34

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
//    setbit(TIMSK0,OCIE0A); //compare A interrupt enable

    // Fast PWM
    TCCR0A |= (1<<WGM01) | (1<<WGM00);


    //load initial servo pos into compare reg
//    OCR0A = startServoPos;

    //external int setup for comms
//    setbit(MCUCR,ISC00); //rising edge of int0
//    setbit(MCUCR,ISC01); //rising edge of int0
    setbit(GIMSK,PCIE); //enable pin change ints
    setbit(PCMSK,PCINT3); //enable pin change int 3 (pb3)
    
    setbit(PORTB,LED_PIN);
    //flash led on powerup
    for( int i = 0; i < 4; i++ )
    {
    clearbit(PORTB,LED_PIN); //turn off led
    _delay_ms(100);
    setbit(PORTB,LED_PIN);
    _delay_ms(100);
    }
    clearbit(PORTB,LED_PIN); //turn off led


    //turn on servo pwm generation 
    OCR0A = PENUP;
    TCCR0A |= (1<<COM0A1);

    //enable interrupts
    sei();

    while(1)
    {
      //interrupts do everything
      _delay_ms(1);

      //if we've been waiting too long then reset back to listen mode
      if( counting == true && counter > 20 )
      {
        counting = false;
        clearbit(PORTB,LED_PIN); //turn off led
      }

    }
}

//pcint 
ISR(PCINT0_vect)
{
    cli();
    if(bit_is_set(PINB,INT_PIN)) //if it was a low to high transition
    {
    //count mode
    if(counting == false)
    {
      counting = true;
      setbit(PORTB,LED_PIN); //led on
      counter = 0;
    }
    //listen mode
    else
    {
      clearbit(PORTB,LED_PIN); //turn off led
      if( counter > 2 && counter <= 4 )
      {
        OCR0A = PENUP;
      }
      else if( counter > 4 && counter <=8)
      {
        OCR0A = PENDOWN;
        //flash the led
        clearbit(PORTB,LED_PIN); //turn off led
        _delay_ms(100);
        setbit(PORTB,LED_PIN);
        _delay_ms(100);
      }
      clearbit(PORTB,LED_PIN); //turn off led
      counter = 0;
      counting = false;
    }
    }
    sei();
}

//timer 0 is used to measure amount of time between power on and pulse over power line
ISR(TIM0_OVF_vect)
{
    //comms counter
    counter ++;
}

