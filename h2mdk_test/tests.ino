/*
test desc:
 test the 3 status leds turn on and off
*/
float test1()
{
  Serial.println( F("test 1") ); 
  Serial.println( F("-----------------------------") );
  int pass =0;
  int tests=3;
  allOff();

  buzz();
  digitalWrite(STATUS_LED,true);
  Serial.println( F("is status led on? (y/n)"));
  pass += readYN();
  digitalWrite(STATUS_LED,false);

  digitalWrite(PURGE,true);
  Serial.println( F("is purge led on? (y/n)"));
  pass += readYN();
  digitalWrite(PURGE,false);

  digitalWrite(SHORT,true);
  Serial.println( F("is short led on? (y/n)"));
  pass += readYN();
  digitalWrite(SHORT,false);
  return float(pass)/tests*100;
}


/*
test desc:
  board starts up with drained caps and using an acceptable current
*/
float test2()
{
  int pass = 0;
  int tests = 5;
  Serial.println( F("test 2") );
  Serial.println( F("-----------------------------") );
  drainCaps();
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, true );
  
  delay(500);
  for( int i = 0; i < tests ; i ++)
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
  return float(pass)/tests*100;
}


/*
test desc:
  output voltage is at 5000mv  
*/
int test3()
{
  int pass=0;
  int tests = 5;
  Serial.println( F("test 3") );
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  delay(500);
  for( int i = 0; i < tests ; i ++)
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
  return float(pass)/tests*100;
}

/*
test desc:
  output voltage is ok with a 1.25W load for 5 seconds 
*/
int test4()
{
  int pass=0;
  int tests = 5;
  Serial.println( F("test 4") );
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );

  chargeCaps();
  connectLoad(Load1_25W);
  delay(500);
  for( int i = 0; i < tests ; i ++)
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
  return float(pass)/tests*100;
}


/*
test desc:
  output voltage is above acceptable limit with a 0.7A load for 5 seconds
*/
int test5()
{
  int pass=0;
  int tests = 5;
  Serial.println( F("test 5") ); 
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );
  chargeCaps();

  connectLoad( Load0_7A );
 
  for( int i = 0; i < tests ; i ++)
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
  return float(pass)/tests*100;
}


/*
test desc:
  output voltage is at acceptable level for a load of 2.75W for 5 minutes
*/
int test6()
{
  int pass=0;
  int tests = 5 * 60; //5 minutes
  Serial.println( F("test 6") ); 
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );

  chargeCaps();
  connectLoad( Load2_75W );

  for( int i = 0; i < tests ; i ++)
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
  return float(pass)/tests*100;
}
/*
test desc:
  temperature is not above 65C anywhere on the board. requires user input
*/
int test7()
{
  Serial.println( F("test 7") ); 
  Serial.println( F("-----------------------------") );
  buzz();
  Serial.println( F("measure max temp on the board (enter integer)"));
  while( Serial.available() == 0 )
  {
    ;;
  }
  int temp = serReadInt();
  Serial.println( temp );
  if( temp > maxTemp )
    return 0;
  return 100;
}
/*
test desc:
  check solenoid is working by measuring current drawn while it's on
*/
int test8()
{
  Serial.println( F("test 8") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  int tests =5 ;
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
  for( int i = 0; i < tests ; i ++ )
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
  return float(pass)/tests*100;
}
/*
test desc:
  check short is working by measuring current drawn while it's on
*/
int test9()
{
  Serial.println( F("test 9") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  int tests = 5;
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
  for( int i = 0; i < tests ; i ++ )
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
  return float(pass)/tests*100;
}

/*
test desc:
  check we can read the voltage using the board's potential divider
*/
int test10()
{
  Serial.println( F("test 10") ); 
  Serial.println( F("-----------------------------") );
  digitalWrite( connectSupply, true );
  delay(1000);
  int pass = 0;
  int tests = 5;
   for( int i = 0; i < tests ; i ++ )
   {
  float v = 2*getAvgAnalogRead(VOLTAGE_SENSE); 
  Serial.println( v );
    if( v > maxSupplyVoltage )
    {
      Serial.println( "voltage too high" );
    }
    else if( v < minSupplyVoltage )
    {
      Serial.println( "voltage too low" );
    }
    else
    {
      pass ++;
    }
    delay(1000);
   }
  digitalWrite( connectSupply, false );
  return float(pass)/tests*100;
}

/*
test desc:
  check we can read the current using the board's current monitor
  check we are not too far out with comparison to our own monitor on all the loads we have available
*/
int test11()
{
  Serial.println( F("test 11") ); 
  Serial.println( F("-----------------------------") );

  //on board current reader relies on getting supply from fuelcell
  digitalWrite( connectSupply, true );
  digitalWrite( LOAD, HIGH );

  delay(500);

  float us;
  float them;
  int pass =0;
  int tests = 7;
  for( int i = 0 ; i < tests; i ++ )
  {
    connectLoad(i);
    delay(1000);
    us = measureSupplyCurrent();
    them = measureFCCurrent();
    if( abs(us - them) < maxCurrentDiff )
    {
      pass ++;
      Serial.println("pass");
    }
    else
    {
      Serial.println("fail");
    }
  }
  
  allOff();
    return float(pass)/tests*100;

}
/*
test desc:
  test board boots ok with fuel cell (and drained caps)
  test output voltage is OK when powered with fuel cell
*/
int test12()
{
  Serial.println( F("test 12") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  int tests =5;
  
  drainCaps();
  prepFuelCell();

  digitalWrite( LOAD, HIGH );
  chargeCaps();
  //manage fuel cell (short and purge) better, for now:
  for( int i = 0; i < tests ; i ++ )
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
  return float(pass)/tests*100;
}


/*
test desc:
  test board output voltage is stable with a 1.25W load for 5 minutes
*/
int test13()
{
  Serial.println( F("test 13") ); 
  Serial.println( F("-----------------------------") );
  int pass = 0;
  int tests = 5 * 60;
  prepFuelCell();
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  prepFuelCell();

  connectLoad(Load1_25W);
  for( int i = 0; i < tests ; i ++ )
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
  return float(pass)/tests*100;
}

/*
test desc:
  do a short while running on fuel cell with 1.25W load. check output voltage doesn't drop too low
*/
int test14()
{
  Serial.println( F("test 14") ); 
  
  Serial.println( F("-----------------------------") );
  int pass = 0;
  int tests = 5;
  prepFuelCell();
  digitalWrite( LOAD, HIGH );
  chargeCaps();
  connectLoad(Load1_25W);
  for( int i = 0; i < tests ; i ++ )
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
  return float(pass)/tests*100;
}
