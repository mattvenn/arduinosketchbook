
const int num_beams = 1;
const int beam_pins[num_beams] = { 2 }; //pins 5&6 seem broken on this leonardo

const int led = 13;

boolean is_read = true;
byte msg;

void setup()
{
  Serial.begin(57600);
  delay(2000);
  Serial.println("started");
  for(int beam=0; beam<num_beams; beam++)
  {
    pinMode(beam_pins[beam],INPUT);
    Serial.print("setting pin:"); Serial.println(beam_pins[beam]);
    digitalWrite(beam_pins[beam],LOW);
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
      {
       // Serial.println(beam);
        msg |= 1 << beam;
      }
    }
    is_read = false;
  }
  
  //allow pc to read status
  if(Serial.available())
  {
    //don't care about the message
    Serial.read();
    Serial.write(msg);
//    Serial.println(msg,BIN);
    is_read = true;
  }
}

void flashLed(int num)
{
  for(int i =0; i<num; i++)
  {
    digitalWrite(led,HIGH);
    delay(10);
    digitalWrite(led,LOW);
    delay(10);
  }
}
