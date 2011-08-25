

#define TRUE 1
#define FALSE 0

// timer interrupt
ISR(TIMER1_COMPA_vect)
{
   cli();
   PORTD |= _BV(PD5);
   PORTD &= ~_BV(PD5);
   sei();
}

void setupTimer( void )
{
//  uint8_t i;


 /*
   * Set up the 16-bit timer 1.
   *
   * Timer 1 will be set up as a 10-bit phase-correct PWM (WGM10 and
   * WGM11 bits), with OC1A used as PWM output.  OC1A will be set when
   * up-counting, and cleared when down-counting (COM1A1|COM1A0), this
   * matches the behaviour needed by the STK500's low-active LEDs.
   * The timer will runn on full MCU clock (1 MHz, CS10 in TCCR1B).
   */
  TCCR1A = 0; //_BV(COM1A0);
  TCCR1B = _BV(WGM12) | _BV(CS10); // ctc mode / no prescaler
  OCR1A = 120;			/* count up to this */

  TIMSK1 = _BV(OCIE1A);

  sei(); /* global interrupt enable */
//  Serial.begin(9600);

  //Serial.println("running");
}

