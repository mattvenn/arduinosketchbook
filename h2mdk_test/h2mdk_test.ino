/* 

still major probs with reading current sensor DANG!!!

*/
#define AREF 3300.0
/* h2mdk defs */
    static const int PURGE = 3;
    static const int LOAD = 4;  // 3w can disconnect load
    static const int SHORT = 5;
    static const int STATUS_LED = 6;

//analog pins
    static const int VOLTAGE_SENSE = A1;
    static const int CURRENT_SENSE = A2;
    static const int CAP_V_SENSE = A3;

/* extra defs */

    static const int supplyCurrent= A4;
    static const int outputVoltage = A0;
    
    static const int connectSupply = 9;
    static const int connectFC = 8;
    static const int connectLoad1 = 10;
    static const int connectLoad2 = 7;
    static const int connectLoad3 = 11;
//elect defs
static const float standbyCurrent = 0.5; //A
static const float maxBootCurrent = 1.0; //A
static const float minOutputVoltage = 4500; //mv
static const float minBootCurrent = 0.3; //A check this
//these both are wrong because of the old board I'm working against
static const float drainedCapVoltage = 4000; //mv
static const float chargedCapVoltage = 6000; //mv

static const float shortCurrent = 1.7; //A depends on psu current limit setting
static const float solenoidCurrent = 0.65; //A check this
   
float currentOffsetV = 0;

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
  calibrateCurrentSensor();
}


void loop()
{
  if( Serial.available() )
  {
    char command = Serial.read();
    
    delay(50);
    
    
    switch( command )
    {
      case 'r': //run all tests
      {
        Serial.println( F("running all tests...") );
        int t2pass = test2();
        int t4pass = test4();
        int t8pass = test8();
        int t9pass = test9();
        //as CSV
        Serial.println( "test2,test4,test8,test9" );
        Serial.print( t2pass ); Serial.print(",");
        Serial.print( t4pass ); Serial.print(",");
        Serial.print( t8pass ); Serial.print(",");
        Serial.print( t9pass ); Serial.print(",");
        Serial.println();
        break; 
        }
      case 't':   //run a test
      {
        int testNum = serReadInt();
        Serial.print( testNum );
        if( testNum == 2 )
          test2();
        else if( testNum == 4)
          test4();
        else if( testNum == 5)
          test5();
        else if( testNum == 8)
          test8();
        else if( testNum == 9)
          test9();
        else if( testNum == 12)
          test12();
        else 
          Serial.println(F("unknown test number"));
        break;
      }
      case 'c':
        drainCaps();
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
  digitalWrite( connectLoad1, false );
  digitalWrite( connectLoad2, false );
  digitalWrite( connectLoad3, false );
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
  
void chargeCaps()
{
  Serial.println( "charge caps" );
  allOff();
  digitalWrite( connectSupply, true );
  
  int count = 0;
  while( measureCapVoltage() < chargedCapVoltage) //todo
  {
    delay(100);
    if( count ++ > 1000 )
      break;
  }
}

void drainCaps()
{
  Serial.println( "drain caps" );
  digitalWrite( LOAD, HIGH );
  digitalWrite( connectSupply, false );
  digitalWrite( connectLoad1, true );
  digitalWrite( connectLoad2, true );
  digitalWrite( connectLoad3, true );
  //externalMosfet( connectLoad2, true );
  while( measureCapVoltage() > drainedCapVoltage ) //todo
  {
    delay(100);
  }
  allOff();
}

float measureCapVoltage()
{
  float v = 2*AREF/1024.0*analogRead(CAP_V_SENSE) ;
//  Serial.print( "cap v: ");
//  Serial.println( v );
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

float measureSupplyCurrent()
{
  float v = getAvgAnalogRead(supplyCurrent); // AREF/1024.0*analogRead(supplyCurrent) ;
  /*Serial.print( ">> v: " );
  Serial.print( v );
  Serial.print( " off: " );
  Serial.println( v - currentOffsetV );
  */
  float i =( v - currentOffsetV ) / 160; // should be 185mv per amp but just ain't working
  Serial.print( "supply i: ");
  Serial.println( i );
  return i;
}
