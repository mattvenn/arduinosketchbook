//pin defs
#define stepDir 3
#define stepPulse 2
#define motorOn 4

//prog defs
#define STEPS 75
#define MINSPEED 3
#define MAXSPEED 30
#define FORWARD false
#define BACKWARD true

//globals
boolean draw = false;

void setup()
{
  pinMode( stepDir, OUTPUT );
  pinMode( stepPulse, OUTPUT );
  pinMode( motorOn, OUTPUT );
  digitalWrite( motorOn, HIGH ); 
  Serial.begin(9600);
  Serial.println( "floppy drawbot starting" );
}

void loop()
{
  if( Serial.available() )
  {
    char command = Serial.read();
    switch( command )
    {
      case 'g':
        draw = true;
        digitalWrite( motorOn, false ); //switch platter motor on
        break;
      case 's':
        draw = false;
        digitalWrite( motorOn, true ); //switch platter motor off
        break;
    }
  }
    
  if( draw )
  {
    doSteps(FORWARD, STEPS, random(MINSPEED,MAXSPEED));
    doSteps(BACKWARD, STEPS, random(MINSPEED,MAXSPEED));
  }
}

void doSteps( boolean dir, int steps, int stepDelay )
{
  digitalWrite( stepDir, dir );
  for( int i = 0; i < steps; i ++ )
  {
    digitalWrite( stepPulse, LOW );
    delay( stepDelay );
    digitalWrite( stepPulse, HIGH );
    delay( stepDelay );
  }
}
  
  
