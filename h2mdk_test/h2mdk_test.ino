/* 
add a higher load
add a way of switching lower supply voltage
3 hours on 17/9
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

    static const int supplyCurrent= A0;
    static const int outputVoltage = A4;
    
    static const int connectSupply = 7;
    static const int connectLoad1 = 8;
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
    
void setup()
{
  Serial.begin(9600);
  Serial.println( "started" );

  pinMode( connectSupply, OUTPUT );
  pinMode( connectLoad1, OUTPUT );
  pinMode(PURGE,OUTPUT);
  pinMode(LOAD,OUTPUT);
  pinMode(SHORT,OUTPUT);
  pinMode(STATUS_LED,OUTPUT);

  allOff();
  analogReference(EXTERNAL);
}


void loop()
{
  if( Serial.available() )
  {
    char command = Serial.read();
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
      case '2': //check op is 5v when we apply power, and supply current is < 0.75
        test2();
        break;
      case '4': //check output current and voltage with a load attached
        test4();
        break;
      case '8': //solenoid
        test8();
        break;
      case '9': //short
        test9();
        break;
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
  externalMosfet( connectLoad1, false );
}
void externalMosfet( int pin, boolean state )
{
  digitalWrite( pin, ! state );
}

void chargeCaps()
{
  Serial.println( "charge caps" );
  digitalWrite( LOAD, LOW );
  digitalWrite( connectSupply, true );
  externalMosfet( connectLoad1, false );
  //externalMosfet( connectLoad2, false );
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
  externalMosfet( connectLoad1, true );
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
  float v = 2*AREF/1024.0*analogRead(outputVoltage) ;
  Serial.print( "output v: ");
  Serial.println( v );
  return v;
}
float measureSupplyCurrent()
{
  float v = AREF/1024.0*analogRead(supplyCurrent) ;
  float i =( v - 5000 / 2 ) / 185; //185mv per amp
  Serial.print( "supply i: ");
  Serial.println( i );
  return i;
}
