#include <EEPROM.h>

/*
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 */

#include <LiquidCrystal.h>

// LiquidCrystal display with:
// rs on pin 12
// rw on pin 11
// enable on pin 10
// d0, d1, d2, d3 on pins 5, 4, 3, 2
//LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);
#include "WProgram.h"
void setup();
void loop();
void doDisplay();
LiquidCrystal lcd(3, 0, 2, 5, 4, 7, 6);
unsigned int water;
unsigned int power;

void setup()
{
 // Serial.begin(9600);
 //nb eeprom stores only chars.
  water = 1; //EEPROM.read( 0 );
  power = 1; //EEPROM.read( 1 );
}

void loop()
{

  water += millis() / 1000;
  power += millis() / 1300;
//  EEPROM.write( water, 0 );
//  EEPROM.write( power, 1 );
  
  doDisplay();
 delay( 1000 );
}

void doDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0); // col, row
  lcd.print( "power: " );
  lcd.print( power );
  lcd.print( " kW/h");
  lcd.setCursor(0,1 );
  lcd.print( "water: " );
  lcd.print( water );
  lcd.print( " L" );
 
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

