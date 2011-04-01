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
LiquidCrystal lcd(3, 0, 2, 5, 4, 7, 6);
unsigned int water;
unsigned int power;

void setup()
{
 // Serial.begin(9600);
  water = EEPROM.read( 0 );
  power = EEPROM.read( 1 );
}

void loop()
{

  int water = millis() / 1000;
  int power = water / 4;
  eeprom.write
  lcd.clear();
  lcd.setCursor(0, 0); // col, row
  lcd.print( "power: " );
  lcd.print( millis() / 1000 );
  lcd.print( " kW/h");
  lcd.setCursor(0,1 );
  lcd.print( "water: " );
  lcd.print( 15 );
  lcd.print( " L" );
  delay( 200 );
}
