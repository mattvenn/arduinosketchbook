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
  Serial.println("ARCOLA ENERGY > HYMERA DIAGNOSTICS");
}

void updateFuelCellStatus () {
  fuelcellSerial.flush();  //  clear buffer 
  while (fuelcellSerial.available() > 0 ) {      // wait for a gap of over 100ms with no byte received
    fuelcellSerial.read();  //     
    delay (100); 
  }

  while (fuelcellSerial.available() <8) {      // wait for buffer to contain 8 bytes - maybe not necessary. i put it in to debug
  }

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
  message.fuelcellOutputCurrent = ((fuelcellRawOutputCurrent1<<8) + fuelcellRawOutputCurrent2) * .1;// 
  message.fuelcellBatteryV = fuelcellBatteryRawV * .10;
  
}  

