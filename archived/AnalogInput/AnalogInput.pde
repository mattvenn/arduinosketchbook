/*
  Analog Input
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 13. 
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead(). 
 
 The circuit:
 * Potentiometer attached to analog input 0
 * center pin of the potentiometer to the analog pin
 * one side pin (either one) to ground
 * the other side pin to +5V
 * LED anode (long leg) attached to digital output 13
 * LED cathode (short leg) attached to ground
 
 * Note: because most Arduinos have a built-in LED attached 
 to pin 13 on the board, the LED is optional.
 
 
 Created by David Cuartielles
 Modified 16 Jun 2009
 By Tom Igoe
 
 http://arduino.cc/en/Tutorial/AnalogInput
 
 */
#include <LiquidCrystal.h>
LiquidCrystal lcd(0, 13, 1, 2, 3, 4, 5);
#define PUMP_SENSE 2 //adc

int sensorPin = 0;    // select the input pin for the potentiometer

int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:

}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
   lcd.clear();
  lcd.print("hcf" );
  lcd.setCursor( 0, 1 );
  lcd.print( sensorValue );
  delay( 100 );
 
}
