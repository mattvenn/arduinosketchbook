//relays spi controller needs <2mhz, so at least clock_div8 as we're running at 16mhz
void relay_setup()
{
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  for(int i = 0; i<4; i ++)
    relay_control(i,false);

}
void test_relays()
{
  for(int d = 350; d > 0; d -= 100)
  {
    for(int i = 0; i<4; i ++)
    {
      relay_control(i,true);
      delay(d);
      relay_control(i,false);
      delay(d);
    }
  }
}



//doesn't preserve other relay's state
//sometimes doesn't work - fixed now that spi clock lowered?
void relay_control(int relay, boolean state)
{
  digitalWrite(relay_cs,LOW);
  if(state)
  {
    //latch it
    SPI.transfer(0b10 << relay * 2);
    digitalWrite(relay_cs,HIGH);

    delay(1);

    //remove drive current
    digitalWrite(relay_cs,LOW);
    SPI.transfer(0b00 << relay * 2);
  }
  else
  {
    //latch
    SPI.transfer(0b01 << relay * 2);
    digitalWrite(relay_cs,HIGH);

    delay(1);

    //remove drive current
    digitalWrite(relay_cs,LOW);
    SPI.transfer(0b00 << relay * 2);
  }
  digitalWrite(relay_cs,HIGH);
}




