
const int num_beams = 1;
const int beam_pins[num_beams] = { 2 };

const int led = 13;

boolean is_read = true;
byte msg;

void setup()
{
  Serial.begin(57600);
  Serial.println("started");
  for(int beam=0; beam<num_beams; beam++)
  {
    pinMode(beam_pins[beam],INPUT);
  }
  pinMode(led,OUTPUT);
  flashLed(2);
}

void loop()
{
  //only check beams if the last state has been read
  if(is_read)
  {
    flashLed(1);
    msg = 0;
    for(int beam=0; beam<num_beams; beam++)
    {
      if( digitalRead(beam_pins[beam]))
        msg |= 1 << beam;
    }
    is_read = false;
  }
  
  //allow pc to read status
  if(Serial.available())
  {
    //don't care about the message
    Serial.read();
    Serial.print(msg,BIN);
    is_read = true;
  }
}

void flashLed(int num)
{
  for(int i =0; i<num; i++)
  {
    digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    delay(100);
  }
}
