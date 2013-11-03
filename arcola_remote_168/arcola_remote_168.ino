/*

 avrdude -p atmega168p -P /dev/ttyACM1  -c STK500 -v -B 2 -U flash:w:./arcola_remote_168.cpp.hex
*/
#define HY_RX 0
#define HY_TX 1
#define MEGA_POWER 8
#define MEGA_RESET 7
void setup()
{
  pinMode(HY_RX,OUTPUT);
  pinMode(HY_TX,OUTPUT);
  pinMode(MEGA_POWER,OUTPUT);
  pinMode(MEGA_RESET,INPUT);
  //float input otherwise can't program mega!
  //digitalWrite(MEGA_RESET,HIGH);
  
  
}
bool state = false;
void loop()
{
  state = ! state;
  digitalWrite(HY_RX,state);
  digitalWrite(HY_TX,state);
 // digitalWrite(MEGA_POWER,state);
 // digitalWrite(MEGA_RESET,state);
  delay(1000);
  
}
