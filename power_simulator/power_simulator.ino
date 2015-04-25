const int loop_time = 2000;

const int led_pin = 13;
boolean led_state = false;
const int switches = 6;
int switch_pins[switches] = {7,6,5,4,3,2};
int power_levels[switches] = {100,100,500,300,2000,1000};

//setup
void setup()
{
  Serial.begin(9600);
  pinMode(led_pin,OUTPUT);

  for( int i = 0; i < switches; i ++)
  {
    pinMode(switch_pins[i],INPUT);
    digitalWrite(switch_pins[i],HIGH);
  }
}

//loop
void loop()
{
  //blink led
  digitalWrite(led_pin,led_state);
  led_state = !led_state;

  int total_power = 0;

  //check switches
  for(int i = 0; i < switches; i ++)
  {
    //switch is on
    if(digitalRead(switch_pins[i])==LOW)
    {
      total_power += power_levels[i];      
    }
  }

  //print the total power
  Serial.print("power=");
  Serial.println(total_power);

  //wait
  delay(loop_time);
}
