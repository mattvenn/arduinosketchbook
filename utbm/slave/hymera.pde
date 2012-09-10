/* DECODE SERIAL OUTPUT OF HYMERA  (12 Jan 2012)

Works with Arduino 0.23 and NewSoftSerial. Could convert to Arduino 1.0 with SoftwareSerial
Had problems using Hardware Serial on a Mega
 
 Connect Hymera service port to MAX232CPE.
 Looking into Pins of DSUB connector, top right (5) is GND. Green wire on the cable I used.
 Second from left top row (2) is DATA. Red wire on the cable I used.
 MAX232 Pinout:
 16: +5V
 15: GND
 13: HYMERA DATA
 12: ARDUINO SOFT SERIAL RX PIN 4
 
 */
#include <avr/pgmspace.h>

//define status strings
prog_char string_0[] PROGMEM = "normal          ";
prog_char string_1[] PROGMEM = "stack temp high";
prog_char string_2[] PROGMEM = "op current high";
prog_char string_3[] PROGMEM = "stk current high";
prog_char string_4[] PROGMEM = "batt < 10.5v";
prog_char string_5[] PROGMEM = "batt < 11v";
prog_char string_6[] PROGMEM = "h2 pressure low";
prog_char string_7[] PROGMEM = "stack V low";
prog_char string_8[] PROGMEM = "manual turn off";
prog_char string_9[] PROGMEM = "fcp temp > 55C";
prog_char string_10[] PROGMEM = "h2low+batt<10.5";

//make a table
PROGMEM const char *string_table[] = 	   // change "string_table" name to suit
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5, 
  string_6, 
  string_7, 
  string_8, 
  string_9, 
  string_10,
  };


//raw vals
int fuelcellAmbientRawT;
int fuelcellStackRawV;
int fuelcellStackRawI;
int fuelcellStackRawT;
int fuelcellRawOutputCurrent1; //high byte
int fuelcellRawOutputCurrent2; //low byte
int fuelcellBatteryRawV;

//data
/*
int fuelcellStatus;
float fuelcellAmbientT;
float fuelcellStackV;
float fuelcellStackI;
float fuelcellStackT;
float fuelcellOutputCurrent;// 
float fuelcellBatteryV;
*/

void printStatus(int message)
{
  char buffer[16];    // make sure this is large enough for the largest string it must hold

    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[message]))); // Necessary casts and dereferencing, just copy. 
    lcd.print(buffer);
    Serial.print("error: ");
    Serial.println(buffer);
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void setupFuelcellSerial()
{
  fuelcellSerial.begin(9600); 
  Serial.println("ARCOLA ENERGY");
}

void updateFuelCellStatus () 
{
  fuelcellSerial.listen();  
  fuelcellSerial.flush();  //  clear buffer 
  while (fuelcellSerial.available() > 0 ) {      // wait for a gap of over 100ms with no byte received
   if( digitalRead(RX) == HIGH )
     return;
   // debug( "read char\n" );
    fuelcellSerial.read();  //     
    delay (100); 
  }
  //debug("found gap\n" );
  while (fuelcellSerial.available() <8) {
       if( digitalRead(RX) == HIGH )
     return;
   //  Serial.println( fuelcellSerial.available() );    // wait for buffer to contain 8 bytes - maybe not necessary. i put it in to debug
  }
  //debug("buffer full\n" );
  message.fuelcellStatus = fuelcellSerial.read();
  fuelcellAmbientRawT = fuelcellSerial.read();
  fuelcellStackRawV = fuelcellSerial.read();
  fuelcellStackRawI = fuelcellSerial.read();
  fuelcellStackRawT= fuelcellSerial.read();
  fuelcellRawOutputCurrent1 = fuelcellSerial.read(); //high byte
  fuelcellRawOutputCurrent2 = fuelcellSerial.read(); //low byte
  fuelcellBatteryRawV = fuelcellSerial.read();

  //calculate actual values
  message.fuelcellAmbientT = fuelcellAmbientRawT * .5;
  message.fuelcellStackV = fuelcellStackRawV * .1;
  message.fuelcellStackI = fuelcellStackRawI * .1;
  message.fuelcellStackT = fuelcellStackRawT * .5;
  message.fuelcellOutputCurrent = ((fuelcellRawOutputCurrent1<<8) + fuelcellRawOutputCurrent2) * .1;
  
}  

