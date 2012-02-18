/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 25 July 2009
 by David A. Mellis
 
 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
#define STATUS_OK 11 //0
#define STATUS_BAD 13 //1
#include "WProgram.h"
void setup();
void loop();
LiquidCrystal lcd(6,20,7,8,9,10,17); //rs, rw (not used), enable, d4->d7
#include <WProgram.h>
#include <Wire.h>
#include <DS1307.h> // written by  mattt on the Arduino forum and modified by D. Sjunnesson

void setup() {
 lcd.begin(20, 4);
   pinMode( STATUS_OK, OUTPUT );
  pinMode( STATUS_BAD, OUTPUT );
 
}

void loop() {
   lcd.clear();
  lcd.print("http://powerpacks." );
  lcd.setCursor( 0,1 );
  lcd.print("ArcolaEnergy.com");
  lcd.setCursor( 0,3 );

 lcd.print(RTC.get(DS1307_HR,true)); //read the hour and also update all the values by pushing in true
  lcd.print(":");
  lcd.print(RTC.get(DS1307_MIN,false));//read minutes without update (false)
  lcd.print(":");
  lcd.print(RTC.get(DS1307_SEC,false));//read seconds
  lcd.print("      ");                 // some space for a more happy life
  lcd.print(RTC.get(DS1307_DATE,false));//read date
  lcd.print("/");
  lcd.print(RTC.get(DS1307_MTH,false));//read month
  lcd.print("/");
  lcd.print(RTC.get(DS1307_YR,false)); //read year 
 

digitalWrite( STATUS_OK, HIGH);
digitalWrite( STATUS_BAD, LOW );
  delay( 100 );
  digitalWrite( STATUS_OK, LOW);
digitalWrite( STATUS_BAD, HIGH );
delay( 100 );
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

