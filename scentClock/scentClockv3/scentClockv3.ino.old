/* 
tested:
  heaters OK
  pots OK
  el_enable not tested
  fan OK but not pwm
*/
const int heaters = 3;
const int heater_pins[heaters] = {5,6,10};

const int pots = 3;
const int pot_pins[pots] = {A0,A1,A2};

const int el_enable = 9;
const int fan = 7;

#define test_time

void setup()
{
  Serial.begin(57400);
  Serial.println("started");
  
  //pin io
  for(int i=0; i<heaters; i++)
    pinMode(heater_pins[i],OUTPUT);
  pinMode(el_enable,OUTPUT);
  pinMode(fan,OUTPUT);
  
  //rtc
  setup_rtc();
}

void loop()
{
  Serial.println("loop");
  #ifdef test_pots
  for(int i=0; i<pots; i++)
     Serial.println(analogRead(pot_pins[i]));
  #endif
  #ifdef test_heaters
  for(int i=0; i<heaters; i++)
  {
    Serial.println(i);
    analogWrite(heater_pins[i],200);
    delay(1000);
    analogWrite(heater_pins[i],0);
    delay(1000);
  }
  #endif
  #ifdef test_fan
  digitalWrite(fan,HIGH);
  delay(1000);
  digitalWrite(fan,LOW);
  #endif
  #ifdef test_time
  print_time();
  delay(1000);
  #endif
}
