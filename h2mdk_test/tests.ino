int test1()
{
  return -1;
}

int test2()
{
  int pass = 0;

  Serial.println( F("test 2") );
  Serial.println( F("-----------------------------") );
  drainCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  
  delay(500);
  for( int i = 0; i < 5 ; i ++)
  {

    float i = measureSupplyCurrent();
    if(i > maxBootCurrent )
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


int test3()
{
  int pass=0;
  Serial.println( F("test 3") );
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  delay(500);
  for( int i = 0; i < 5 ; i ++)
  {
    if( measureOutputVoltage() > minOutputVoltage )
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

int test4()
{
  int pass=0;
  Serial.println( F("test 4") );
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );

  chargeCaps();
  connectLoad(Load1_25W);
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
    delay(2000);
  }
  allOff();
  return pass;
}


int test5()
{
  int pass=0;
  Serial.println( F("test 5") ); 
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();

  connectLoad( Load0_7A );
 
  for( int i = 0; i < 5 ; i ++)
  {
     if( measureOutputVoltage() < minOutputVoltageMaxLoad )
     {
       Serial.println( F( "op voltage too low" ));
     }
     else
     {
       pass ++;
      Serial.println( "pass" );
     }
     delay(1000);
  }
  
  allOff();
  return pass;
}


int test6()
{
  int pass=0;
  Serial.println( F("test 6") ); 
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );

  chargeCaps();
  connectLoad( Load2_75W );

  for( int i = 0; i < 5 ; i ++)
  {
     if( measureOutputVoltage() < minOutputVoltageMaxLoad )
     {
       Serial.println( F( "op voltage too low" ));
     }
     else
     {
       pass ++;
      Serial.println( "pass" );
     }
     delay(1000);
  }
  
  allOff();
  return pass;
}
//temp measure
int test7()
{
  Serial.println( F("test 7") ); 
  Serial.println( F("-----------------------------") );
  return -1;
}
int test8()
{
  Serial.println( F("test 8") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  Serial.println( "solenoid" );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();

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
  Serial.println( F("test 9") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  Serial.println( "short circuit" );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();
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

//test 10 and 11 are about reading the voltag and current sensor
int test10()
{
  Serial.println( F("test 10") ); 
  Serial.println( F("-----------------------------") );
  return -1;
}
int test11()
{
  Serial.println( F("test 11") ); 
  Serial.println( F("-----------------------------") );
  return -1;
}
//with fuel cells
int test12()
{
  Serial.println( F("test 12") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  prepFuelCell();
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  //manage fuel cell (short and purge) better, for now:
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


int test13()
{
  Serial.println( F("test 13") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  prepFuelCell();
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  prepFuelCell();

  connectLoad(Load1_25W);
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

int test14()
{
  Serial.println( F("test 14") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  prepFuelCell();
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  connectLoad(Load1_25W);
  for( int i = 0; i < 5 ; i ++ )
  {
    digitalWrite( SHORT, HIGH );
  delay(50);

    if( measureOutputVoltage() > minOutputVoltage )
    {
      Serial.println("pass");
      pass ++;
    }
    else
    {
      Serial.println("fail");
    }

  digitalWrite( SHORT, LOW);

    delay(1000);
  }

  allOff();
  return pass;
}
