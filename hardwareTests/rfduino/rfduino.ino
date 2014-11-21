#include <RFduinoBLE.h>
int count = 0;
const int num_leds = 4;
int leds[num_leds] = {2,3,4,5};
/* default serial RX & TX are on pins 0 & 1 */
int motor = 0;
int motor_on = 50;
int motor_off = 300;

void setup() {
  RFduinoBLE.advertisementData = "temp";

 // Serial.begin(9600);
 // Serial.println(num_leds);

  // start the BLE stack
  RFduinoBLE.begin();
  for( int i = 0 ; i<num_leds; i ++)
  {
      pinMode(leds[i],OUTPUT);
      digitalWrite(leds[i],HIGH);
      //Serial.println(i);
  }
  pinMode(motor,OUTPUT);
  digitalWrite(motor,LOW);
}

void loop() {
  // sample once per second
  RFduino_ULPDelay( SECONDS(1) );


// Serial.println(count,HEX);
  RFduinoBLE.sendInt(count++);
}

void RFduinoBLE_onReceive(char *data, int len)
{
  //Serial.println(len);
  //Serial.print(data[0],HEX);
  if(len == 2)
  {
      indicate(data[0],data[1]);
  }  
 //   analogWrite(led, data[0]);
}
void indicate(int start, int end)
{
  if( start > end )
  {
      for(int i = start; i <= end; i ++)
      {
        bar_graph(i);
        vibe();
      }
  }
  else
  {
      for(int i = start; i >= end; i --)
      {
        bar_graph(i);
        vibe();
      }
  }

  //lights off
  bar_graph(0);
}

void vibe()
{
    digitalWrite(motor,HIGH);
    delay(motor_on);
    digitalWrite(motor,LOW);
    delay(motor_off);
}
void bar_graph(int level)
{
  //all off
  for(int i=0; i < num_leds; i ++)
        digitalWrite(leds[i],LOW); 

  //turn on what we need
  for(int i=0; i < level; i ++)
    digitalWrite(leds[i],HIGH); 
}
