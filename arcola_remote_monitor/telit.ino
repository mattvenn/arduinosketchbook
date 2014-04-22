void setup_telit()
{
  Serial1.begin(9600);
}

//test the output pins by putting a jumper between pins 2&3 on telit port. 
void test_telit_232()
{
  pinMode(telit_tx,OUTPUT);
  pinMode(telit_rx,INPUT);
  pinMode(lcd_rx,INPUT);
  for(int i = 0; i < 10; i ++)
  {
    digitalWrite(telit_tx,HIGH);
    Serial.print("true -> ");
    Serial.println(digitalRead(telit_rx));
    //Serial.println(digitalRead(lcd_rx));


    delay(1000);

    digitalWrite(telit_tx,LOW);
    Serial.print("false -> ");
    Serial.println(digitalRead(telit_rx));
    //Serial.println(digitalRead(lcd_rx));


    delay(1000);
  }
}


