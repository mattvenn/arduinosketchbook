const int buttons = 4;
int button_pins[buttons] = {13,12,11,10};
boolean button_states[buttons];
void setup()
{
  Serial.begin(9600);
  for( int i = 0; i < buttons; i ++)
  {
    pinMode(button_pins[i],INPUT);
    digitalWrite(button_pins[i],HIGH);
    button_states[i] = false;
  }
}

void loop()
{

  for(int i = 0; i < buttons; i ++)
  {
    if(digitalRead(button_pins[i])==LOW && button_states[i] == false)
    {
      button_states[i] = true;
      Serial.println(i);
      delay(10); //debounce
    }
    if(digitalRead(button_pins[i])==HIGH)
    {
      button_states[i] = false;
    }
  }
}
