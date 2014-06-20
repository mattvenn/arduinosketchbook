#define LED A1
#define CUR A0

//just looked at the numbers and guessed - not in real units
#define thresh 10000
//turn off after 2 second
#define off_delay 2000


unsigned long time = 0;

void setup()
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  Serial.begin(9600);
}

void loop()
{
  int sample;
  int offset = 512;
  
  double accum = 0;
  for( int i =0; i< 250; i++)
  {
    sample = analogRead(CUR) - offset;
    accum = accum + sample * sample;
  }
  //Serial.println(accum);
  if(accum > thresh)
  {
     digitalWrite(LED,HIGH);
     time = millis();
    Serial.println("on");
  }
  else if(millis() > time + off_delay)
  {
    digitalWrite(LED,LOW);
    Serial.println("off");
  }
  delay(100);
}
