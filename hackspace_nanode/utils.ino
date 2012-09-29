#ifdef MEMTEST
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() 
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}
#endif

// from http://nanodekit.googlecode.com/svn-history/r9/trunk/WatchdogTest.ino
//initialize watchdog
void WatchdogSetup(void)
{
	//disable interrupts
	cli();
	//reset watchdog
	wdt_reset();
	

	// WDTCSR = WatchDog Timer Control Register
	WDTCSR = (1<<WDCE)|(1<<WDE);
	
	//Start watchdog timer with 4s prescaller
	// WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP3)| (0<<WDP2) | (0<<WDP1) | (0<<WDP0);
	
	//Start watchdog timer with 8s prescaller
	WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
	
	//Enable global interrupts
	sei();
}

//Watchdog timeout ISR
ISR(WDT_vect)
{
	//Burst 0.1Hz pulses
	for (int i=0; i < 200; i++) 
	{ 
		digitalWrite(6, LOW);
		delay (250);
		digitalWrite(6, HIGH);
		delay (250);
	} 
	
	Serial.println(F("timeout after 8 seconds"));
	
	WatchdogSetup(); // This is only for this TEST code. If not there, LOOP will hang
	
	// Reboot when TimeOut function done several times
	if ( TimeOutLoopsBeforeReboot-- <= 0)  
	{
		// (B) This is only to insert for your FULL code
		// ----------------------------------------------
		Serial.print(F("\nREBOOTING....\n\n"));
		delay(1000); // leave some time for printing on serial port to complete
		software_Reset();
	}
	
}

void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
	asm volatile ("  jmp 0");  
} 
