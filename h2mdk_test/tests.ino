int test2()
{
  int pass = 0;

  Serial.println( "test 2" );
  drainCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  digitalWrite( connectLoad1, false );
  delay(500);
  for( int i = 0; i < 5 ; i ++)
  {
    float v= measureOutputVoltage();
    float i = measureSupplyCurrent();
    if( v < minOutputVoltage )
    {
      Serial.println( F( "fail - voltage too low")  );
    }
    else if(i > maxBootCurrent )
    {
      Serial.println( F("fail - current too high") );
    }
    else if( i < minBootCurrent )
    {
      Serial.println( F("fail - current too low, check ref voltage") );
    }
    else
    {
      Serial.println( F("pass" ) );
      pass++;
    }
    delay(1000);

  }
  allOff();
  return pass;
}

int test4()
{
  int pass=0;
  Serial.println( "test 4" );
  chargeCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  digitalWrite( connectLoad1, true );
  delay(500);
  for( int i = 0; i < 5 ; i ++)
  {
    if( measureOutputVoltage() > minOutputVoltage && measureSupplyCurrent() > 0.6 )
    {
      Serial.println( "pass" );
      pass++;
    }
    else
    {
      Serial.println( "fail" );
    }
    delay(1000);
  }
  allOff();
  return pass;
}


int test5()
{
  int pass=0;
  Serial.println( "test 5" ); 
   digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  digitalWrite( connectLoad1, true );
 /* digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  digitalWrite( connectLoad1, true );
  delay(500);
  */
  for( int i = 0; i < 50 ; i ++)
  {
    
measureSupplyCurrent();
 measureOutputVoltage();
       delay(500);
  }
  
  allOff();
  return pass;
}


int test8()
{
  int pass = 0;
  Serial.println( "solenoid" );
  chargeCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  delay(500);
  if( measureSupplyCurrent() > standbyCurrent )
  {
    Serial.println( "supply current too high" );
    allOff();
    return 0;
  }
  for( int i = 0; i < 5 ; i ++ )
  {
    digitalWrite( PURGE, HIGH );
    delay(50);
    if( measureSupplyCurrent() > solenoidCurrent)
    {
      Serial.println("pass");
      pass++;
    }
    else
    {
      Serial.println("fail");
    }
    digitalWrite( PURGE, LOW );
    delay(1000);
  }
  allOff();
  return pass;
}
int test9()
{
  int pass = 0;
  Serial.println( "short circuit" );
  chargeCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  delay(500);
  if( measureSupplyCurrent() > standbyCurrent  )
  {
    Serial.println( "supply current too high" );
    allOff();
    return 0;
  }
  for( int i = 0; i < 5 ; i ++ )
  {
    digitalWrite( SHORT, HIGH );
    delay(50);
    if( measureSupplyCurrent() > shortCurrent )
    {
      Serial.println("pass");
      pass ++;
    }
    else
    {
      Serial.println("fail");
    }
    digitalWrite( SHORT, LOW );
    delay(1000);
  }
  allOff();
  return pass;
}

int test12()
{
  int pass = 0;
  //manage fuel cell (short and purge) better, for now:
  digitalWrite(connectFC,HIGH);

  digitalWrite( SHORT, HIGH );
  delay(50);
  digitalWrite( SHORT, LOW);
  digitalWrite( PURGE, HIGH );
  delay(50);
  digitalWrite( PURGE, LOW );
  delay(1000);  
  for( int i = 0; i < 5 ; i ++ )
  {
    measureSupplyCurrent();
    if( measureOutputVoltage() > minOutputVoltage )
    {
      Serial.println("pass");
      pass ++;
    }
    else
    {
      Serial.println("fail");
    }
    delay(1000);
  }

  allOff();
  return pass;
}

