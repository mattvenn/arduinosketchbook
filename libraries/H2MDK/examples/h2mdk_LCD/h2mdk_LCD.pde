/*
example program for running a fuel cell stack
for more info see the website:
http://www.arcolaenergy.com/h2mdk
*/

#include <h2mdk.h>
#include "nokia_3310_lcd.h"
#include<stdlib.h>

//nokia LCD setup - see http://www.nuelectronics.com/estore/index.php?main_page=product_info&products_id=12
Nokia_3310_lcd lcd=Nokia_3310_lcd();

//pass V3W, V12W or V30W depending on your board and fuelcell
//for greater accuracy set second argument to arduino +5V in mV
h2mdk fuelcell(V3W,5000); 

//global vars
char floatVal[8];
int counter = 0;

void setup()
{
  Serial.begin(9600);
  
  //start LCD
  lcd.LCD_3310_init();
  lcd.LCD_3310_clear();
  lcd.LCD_3310_write_string(0, 0, "ArcolaEnergy", MENU_HIGHLIGHT );
  lcd.LCD_3310_write_string(0, 1, "waiting for caps..", MENU_NORMAL );

  //this blocks until caps are charged
  fuelcell.start();
}

void loop()
{
  //this takes care of short circuit, purging and updating electrical values
  //call more often than 100ms
  fuelcell.poll();
  
  //update LCD about every second
  delay( 10 );
  if( counter ++ > 100 )
  {
    updateLCD();
    counter = 0;
  }
}

void updateLCD()
{
  lcd.LCD_3310_clear();
  lcd.LCD_3310_write_string(0, 0, "FC status", MENU_HIGHLIGHT );

  lcd.LCD_3310_write_string(0, 1, "V: ", MENU_NORMAL );
  dtostrf(fuelcell.getVoltage(), 4, 2, floatVal );
  lcd.LCD_3310_write_string(8, 1, floatVal, MENU_NORMAL );
  
  lcd.LCD_3310_write_string(0, 2, "I: ", MENU_NORMAL );
  dtostrf(fuelcell.getCurrent(), 4, 2, floatVal );
  lcd.LCD_3310_write_string(8, 2, floatVal, MENU_NORMAL );
}

