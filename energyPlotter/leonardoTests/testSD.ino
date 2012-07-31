void test_SD()
{
  digitalWrite(SD_SEL,LOW);
  digitalWrite(MISO,HIGH);
  delay(200);
    digitalWrite(MISO,LOW);
    delay(200);
  
}
