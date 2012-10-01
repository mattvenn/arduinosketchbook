/* 
 todo
 
 still major probs with reading current sensor DANG!!!
  -fixed with a fudge factor
 test cap sense before relying on it to work?
 
 
 */

//test specifications
static const float standbyCurrent = 0.5; //A
static const float maxBootCurrent = 1.0; //A
static const float maxCurrentDiff = 0.20; //A max allowable difference between our current sense and the boards
static const float minOutputVoltage = 4800; //mv
static const float minOutputVoltageMaxLoad = 4200; //mv
static const float minBootCurrent = 0.3; //A check this
static const float drainedCapVoltage = 500; //mv
static const float chargedCapVoltage = 4500; //mv 
static const float shortCurrent = 1.7; //A depends on psu current limit setting
static const float solenoidCurrent = 0.65; //A check this
static const int minSupplyVoltage = 1950;
static const int maxSupplyVoltage = 2050;
static const int maxTemp = 65;


/* h2mdk pin defs */
#define AREF 3300.0
static const int PURGE = 3;
static const int LOAD = 4;  // 3w can disconnect load
static const int SHORT = 5;
static const int STATUS_LED = 6;

static const int VOLTAGE_SENSE = A1;
static const int CURRENT_SENSE = A2;
static const int CAP_V_SENSE = A3;

//analog pins
static const int outputVoltage = A0;
static const int supplyCurrent= A4;
//pin defs
static const int connectLoad2 = 7;
static const int connectFC = 8;
static const int connectSupply = 9;
static const int connectLoad1 = 10;
static const int connectLoad3 = 11;
static const int buzzerPin = 12;

//define loads for the main tests
static const int Load1_25W = 2;
static const int Load2_75W = 6;
static const int Load0_7A = 7;

float currentOffsetV = 0;
String serialStr = "";

void setup()
{
  Serial.begin(9600);
  Serial.println( "started" );

  pinMode( connectSupply, OUTPUT );
  pinMode( connectFC, OUTPUT );
  pinMode( connectLoad1, OUTPUT );
  pinMode( connectLoad2, OUTPUT );
  pinMode( connectLoad3, OUTPUT );

  pinMode(PURGE,OUTPUT);
  pinMode(LOAD,OUTPUT);
  pinMode(SHORT,OUTPUT);
  pinMode(STATUS_LED,OUTPUT);

  allOff();

  analogReference(EXTERNAL);
  delay(1500);
  calibrateCurrentSensor();
  calibrateSupplyVoltage();
  Serial.println(F("ready..."));
}


void loop()
{
  if( Serial.available() )
  {
    char command = Serial.read();

    delay(50);


    switch( command )
    {
    case 'a': //run all tests
      {
        getSerial();
        waitForBoard();
        Serial.println( F("running all tests...") );
        int t1 = test1();
        int t2 = test2();
        int t3 = test3();
        int t4 = test4();
        int t5 = test5();
        int t6 = test6();
        int t7 = test7();
        int t8 = test8();
        int t9 = test9();
        int t10 = test10();
        int t11 = test11();
        int t12 = test12();        
        int t13 = test13();        
        int t14 = test14();                
        
        buzz();
        Serial.println( F("complete") );

        Serial.print( serialStr ); Serial.print(",");
        Serial.print( t1 ); Serial.print(",");
        Serial.print( t2 ); Serial.print(",");
        Serial.print( t3 ); Serial.print(",");
        Serial.print( t4 ); Serial.print(",");
        Serial.print( t5 ); Serial.print(",");
        Serial.print( t6 ); Serial.print(",");
        Serial.print( t7 ); Serial.print(",");
        Serial.print( t8 ); Serial.print(",");
        Serial.print( t9 ); Serial.print(",");
        Serial.print( t10 ); Serial.print(",");
        Serial.print( t11 ); Serial.print(",");
        Serial.print( t12 ); Serial.print(",");        
        Serial.print( t13 ); Serial.print(",");                
        Serial.print( t14 ); Serial.print(",");                

        Serial.println();
        break; 
      }
    case 't':   //run a test
      {
        int testNum = serReadInt();
        Serial.print( testNum );
        if( testNum == 1 )
          Serial.println( test1() );
        else if( testNum == 2 )
          Serial.println( test2() );
        else if( testNum == 3 )
          Serial.println( test3() );
        else if( testNum == 4)
          Serial.println( test4() );
        else if( testNum == 5)
          Serial.println( test5() );
        else if( testNum == 6)
          Serial.println( test6() );
        else if( testNum == 7)
          Serial.println( test7() );
        else if( testNum == 8)
          Serial.println( test8() );
        else if( testNum == 9)
          Serial.println( test9() );
        else if( testNum == 10)
          Serial.println( test10() );
        else if( testNum == 11)
          Serial.println( test11() );
        else if( testNum == 12)
          Serial.println( test12() );
        else if( testNum == 13)
          Serial.println( test13() );
        else if( testNum == 14)
          Serial.println( test14() );  
        else 
          Serial.println(F("unknown test number"));
        break;
      }
    default:
      Serial.println( "bad test" );
      break;
    }
  }
}

void allOff()
{
  digitalWrite( LOAD, LOW );

  digitalWrite( connectSupply, false );
  digitalWrite( connectFC, false );
  connectLoad(0);
}

void calibrateCurrentSensor()
{
  allOff();
  int avgNum = 10;
  float avg = 0; 
  Serial.println(F("calibrating current sensor"));
  for( int i = 0; i < avgNum; i ++ )
  {
    avg += AREF/1024.0*analogRead(supplyCurrent) ;
    delay(50);
  }
  avg /= avgNum;
  Serial.println( avg );
  currentOffsetV = avg;
  
}


void connectLoad(int loadNum)
{
  digitalWrite( connectLoad1, false );
  digitalWrite( connectLoad2, false );
  digitalWrite( connectLoad3, false );
  Serial.print( F("load connected: " ) );
  Serial.println(loadNum);
  switch( loadNum )
  {
  case 0:
    ;;
    break;
  case 1:
    digitalWrite(connectLoad3,true);
    break;
  case 2:
    digitalWrite(connectLoad1,true);
    break;
  case 3:
    digitalWrite(connectLoad2,true);
    break;
  case 4:
    digitalWrite(connectLoad3,true);
    digitalWrite(connectLoad1,true);
    break;
  case 5:
    digitalWrite(connectLoad3,true);
    digitalWrite(connectLoad2,true);
    break;
  case 6:
    digitalWrite(connectLoad1,true);
    digitalWrite(connectLoad2,true);
    break;
  case 7:
    digitalWrite(connectLoad1,true);
    digitalWrite(connectLoad2,true);    
    digitalWrite(connectLoad3,true);
    break;
  }
}

//need to connect supply first
void chargeCaps()
{
  Serial.println( "charge caps" );
  connectLoad(0);
 // digitalWrite( connectSupply, true );

  int count = 0;
  while( measureCapVoltage() < chargedCapVoltage) //todo
  {
    delay(100);
    /*
    if( count ++ > 500 )
    {
      Serial.print( "gave up at " );
      Serial.println( measureCapVoltage() );
      break;
    }
    */
  }

}


void drainCaps()
{
  Serial.println( "drain caps" );
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, false );
  connectLoad(7); //all on
  
  while( measureCapVoltage() > drainedCapVoltage ) //todo
  {
    delay(100);
  }
  allOff();
}

float measureCapVoltage()
{
  float v = 2 * AREF/1024.0*analogRead(CAP_V_SENSE) ; //will need todo
    Serial.print( "cap v: ");
    Serial.println( v );
  return v;
}
float measureOutputVoltage()
{
  float v = 2 * getAvgAnalogRead(outputVoltage) ;
  Serial.print( "output v: ");
  Serial.println( v );
  return v;
}


float getAvgAnalogRead(int pin)
{
  unsigned long int v = 0;
  int samples = 100;
  for( int i = 0; i < samples; i ++ )
  {
    v += analogRead(pin) ;
  }
  v /= samples;
  return AREF/1024.0*v;
}

float measureFCCurrent()
{
  float v = getAvgAnalogRead(CURRENT_SENSE);
  float i =( v - 2500 ) / 185; // should be 185mv per amp but just ain't working
  Serial.print( "fc i: ");
  Serial.println( i );
  return i;
}

float measureSupplyCurrent()
{
  float v = getAvgAnalogRead(supplyCurrent); // AREF/1024.0*analogRead(supplyCurrent) ;
  #ifdef CURRENTDEBUG
   Serial.print( ">> v: " );
   Serial.print( v );
   Serial.print( " off: " );
   Serial.println( v - currentOffsetV );
   #endif
   
  float i =( v - currentOffsetV ) / 185; 
  // should be 185mv per amp but just ain't working
  i *= 1.32; //fudge factor to deal with the above.
  Serial.print( "supply i: ");
  Serial.println( i );
  return i;
}

//manage fuel cell (short and purge) better, for now:
void prepFuelCell()
{
 
  digitalWrite(connectFC,HIGH);
  digitalWrite( SHORT, HIGH );
  delay(50);
  digitalWrite( SHORT, LOW);
  digitalWrite( PURGE, HIGH );
  delay(50);
  digitalWrite( PURGE, LOW );
  delay(1000);  
  
}

void calibrateSupplyVoltage()
{
  Serial.println(F("calibrating supply voltage, type y when supply is 2.0v"));
  digitalWrite(connectSupply,true);
  readYN();
/*
  delay(200);

  while( true )
  {
    float v = 2 * getAvgAnalogRead(VOLTAGE_SENSE);
    Serial.println( v );
    if( v > maxSupplyVoltage )
    {
      Serial.println( "supply voltage too high" );
    }
    else if( v < minSupplyVoltage )
    {
      Serial.println( "supply voltage too low" );
    }
    else
    {
      break;
    }
    delay(1000);
  }
*/
  digitalWrite(connectSupply,false);
}

