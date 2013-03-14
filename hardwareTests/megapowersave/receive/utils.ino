void sleep_now()         // here we put the arduino to sleep
{
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
    
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings 
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we 
     * choose the according 
     * sleep mode: SLEEP_MODE_PWR_DOWN
     */  

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 
   /*
     * 
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.   
     * 
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
/*
    //only attach the gas pulse interrupt if the pin has gone high
    if( digitalRead( GAS_PULSE_PIN ) )
    {
 //     Serial.println( "attaching gas pulse pin int" ); delay(100);
      attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function

      // wakeUpNow when pin 2 gets LOW 
    }
    
*/
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
 //   detachInterrupt(0);      // disables interrupt 0 on pin 2 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
                             

}
/*
//ISRS
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
  Serial.println( "watchdog wake" );
}
*/
void wakeUpNow()        // here the interrupt is handled after wakeup
{ 
   detachInt();
   //cli();
   for( int i = 0; i < capture; i ++ )
  {
    buff[i]=digitalRead(0);
    delayMicroseconds(50);
  }
}



//****************************************************************

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms

// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

void setup_watchdog(int ii)
{
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
 // Serial.println(ww);

  MCUSR &= ~(1<<WDRF);

  // start timed sequence
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // set new watchdog timeout value
  WDTCSR = bb;
  WDTCSR |= _BV(WDIE);

}

//****************************************************************  

// Watchdog Interrupt Service / is executed when  watchdog timed out


