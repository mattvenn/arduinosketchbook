void test_analog()
{
  for( int i = 0; i < 10; i ++)
  {
    Serial.print("current 1=");
    Serial.println(analogRead(current_sense_1));

    Serial.print("current 2=");
    Serial.println(analogRead(current_sense_2));

    Serial.print("batt=");
    Serial.println(analogRead(batt_sense));

    delay(1000);
  }

}

