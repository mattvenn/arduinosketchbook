/*

 avrdude -p atmega168p -P /dev/ttyACM1  -c STK500 -v -B 2 -U flash:w:./arcola_remote_168.cpp.hex
*/
#define HY_RX 0
#define HY_TX 1
#define MEGA_POWER 8
#define MEGA_RESET 7

byte count = 0;

void setup()
{
  pinMode(HY_RX,INPUT);
  digitalWrite(HY_RX,HIGH);
  pinMode(HY_TX,OUTPUT);
  pinMode(MEGA_POWER,OUTPUT);
  pinMode(MEGA_RESET,INPUT);
  //float input otherwise can't program mega!
  //digitalWrite(MEGA_RESET,HIGH);
  setup_spi();
  
}
bool state = false;
byte command = 0;

void loop()
{
 // state = ! state;
 // digitalWrite(HY_RX,state);
 // digitalWrite(HY_TX,state);
 // digitalWrite(MEGA_POWER,state);
 // digitalWrite(MEGA_RESET,state);
 // delay(1000);
  
  if(digitalRead(HY_RX)==false)
  {
    count ++;
    delay(100);
  }
  // if SPI not active, clear current command
  if (digitalRead (SS) == HIGH)
    command = 0;
}  // end of loop
  

