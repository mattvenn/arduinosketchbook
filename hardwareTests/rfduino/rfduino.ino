#include <RFduinoBLE.h>

int count = 0;
const int num_leds = 4;
int leds[num_leds] = {2,3,4,5};
// default serial RX & TX are on pins 0 & 1 
int batt_level = 1;
int button = 0;
int motor = 6;
int motor_on = 50;
int motor_off = 300;

int last_reading = 0;

// #define SERIAL_DEBUG
void setup() {
  RFduinoBLE.advertisementData = "temp";
    pinMode(button,INPUT_PULLUP);
    pinMode(batt_level,OUTPUT);
    digitalWrite(batt_level,LOW);
    RFduino_pinWake(button,LOW);
    #ifdef SERIAL_DEBUG
        Serial.begin(9600);
    #endif

  // start the BLE stack
  RFduinoBLE.begin();
   pinMode(motor,OUTPUT);
  digitalWrite(motor,LOW);
  for( int i = 0 ; i<num_leds; i ++)
  {
      pinMode(leds[i],OUTPUT);
     
  }
indicate(1,4);
}

int readDAC()
{
  // battery read stuff
  NRF_ADC->TASKS_START = 1;
  pinMode(batt_level,INPUT);
  delay(10);
  //this adds about 300ua to draw
  int batt = analogRead(batt_level); 
  NRF_ADC->TASKS_STOP = 1;
  pinMode(batt_level,OUTPUT);
  digitalWrite(batt_level,LOW);
  return batt;
}
void loop() {
  // sleeep till we're woken by BLE
  RFduino_ULPDelay(INFINITE );

  //if button pressed, show last reading
  if(RFduino_pinWoke(button))
  {
    RFduino_resetPinWake(button);
    bar_graph(last_reading);
    delay(500);
    bar_graph(0);
  }
}

void RFduinoBLE_onReceive(char *data, int len)
{
 
  if(len == 2)
  {
    #ifdef SERIAL_DEBUG
    Serial.println(data[0],DEC);
    Serial.println(data[1],DEC);
    #endif
      last_reading = data[1];
      indicate(data[0],data[1]);
  }  
 //   analogWrite(led, data[0]);
    RFduinoBLE.sendInt(readDAC());
}
void indicate(int start, int end)
{
  if( start < end )
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
/* not sure if this is needed
void RFduinoBLE_onConnect()
{
  // request central role use a different connection interval in the given range
  // the central role may reject the request (or even pick a value outside the range)
  // we will request something in the 900ms to 1100ms range
  // the actual rate the iPhone uses is 1098ms
  // the best way to get the connection interval you are after is trail and error
  // if the iPhone rejects the request, the connection interval will be the default (25ms)
  RFduinoBLE.updateConnInterval(900, 1100);

  // note: you cannot use delay()/RFduinoBLE.getConnInterval() here to determine which
  // connection interval the iPhone selected - getConnInterval() must be called from
  // either loop() or onReceive()
}
*/
