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

void setup()
{
  pinMode( stepDir, OUTPUT );
  pinMode( stepPulse, OUTPUT );
  pinMode( motorOn, OUTPUT );
  digitalWrite( motorOn, LOW ); //low starts the platter motor
  Serial.begin(9600);
}

void loop()
{

  doSteps(FORWARD, STEPS, random(MINSPEED,MAXSPEED));
  doSteps(BACKWARD, STEPS, random(MINSPEED,MAXSPEED));
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
  
  
