void setup()
{
  //Serial1.begin(9600, SERIAL_8N2);
  Serial1.begin(9600);
    Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(0,INPUT);
}
void loop()
{
 
    if(Serial1.available())
    {Serial.print("got:");
    while(Serial1.available())
    {
      Serial.print((char)Serial1.read());
    }
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
    }
 

 }


