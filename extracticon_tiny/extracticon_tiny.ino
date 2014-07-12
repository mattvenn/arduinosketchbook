#define LED 0
#define CUR A0
#define TRIAC 3

//just looked at the numbers and guessed - not in real units
#define thresh 10000
//turn off after 2 second
#define off_delay 2000


unsigned long time = 0;

void setup()
{
  pinMode(LED,OUTPUT);
  pinMode(TRIAC,OUTPUT);
  digitalWrite(TRIAC,HIGH);
  digitalWrite(LED,HIGH);
  for( int i = 0; i < 4; i ++)
  {
      digitalWrite(LED,LOW);
      delay(100);
      digitalWrite(LED,HIGH);
      delay(100);
  }
  //Serial.begin(9600);

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
 
  if(accum > thresh)
  {
     digitalWrite(LED,HIGH);
     time = millis();
    //Serial.println("on");
  }
  else if(millis() > time + off_delay)
  {
    digitalWrite(LED,LOW);
    //Serial.println("off");
  }
  delay(100);
}
