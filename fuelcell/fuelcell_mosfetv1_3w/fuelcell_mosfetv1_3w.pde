#include <EEPROM.h>
#define SHORT 4
//#define DISC 2
#define LOAD 6
#define PURGE 2
#define LED 13
#define LED2 13
#define DEL 5

#define CAP_V_SENSE A4
#define VOLTAGE_SENSE A5
#define CURRENT_SENSE A2

#define SHORTDEL 100
#define PURGEDEL 100
#define PREPURGE 200 //to ensure we don't purge while shorting!

#define CAP_V 800

//185mv/A
#include <TimedAction.h>

boolean LED2STATE = true;
//this initializes a TimedAction class that will change the state of an LED every second.
TimedAction blinkAction = TimedAction(500,blink);
TimedAction shortAction = TimedAction(5000,shortC);
TimedAction purgeAction = TimedAction(10000,purge);


void setup()
{
  Serial.begin(9600);
  Serial.println( "beginning arcola fuel cell controller" );
  pinMode(LED, OUTPUT);
  //charge pump waveform
 // analogWrite( 3, 128 );
  pinMode( LOAD, OUTPUT );
  digitalWrite( LOAD, HIGH );
  pinMode( SHORT, OUTPUT );
  digitalWrite( SHORT, LOW ); 
  //  pinMode( DISC, OUTPUT );
  //  digitalWrite( DISC, LOW );
  pinMode( LED2, OUTPUT );
  digitalWrite( LED2, LOW );
  pinMode( PURGE, OUTPUT );
  digitalWrite( PURGE, LOW );

  Serial.println( "waiting for caps to charge" );
  //wait for cap to charge

 // checkCaps();
  // delay(10000);


}

void loop()
{
  blinkAction.check();
  shortAction.check();
  purgeAction.check();

}

void checkCaps()
{
  for( int i = 0; i < 20; i ++ )
  {
    blink();
    delay(50);
  }

  while( analogRead( CAP_V_SENSE ) < CAP_V )
  {
    Serial.println( analogRead( CAP_V_SENSE ) );
    delay(100);
    blink();
  }
  Serial.println( "done" );
}
/*
  int addr = 0;
 while( addr < 512 )
 {
 int val = analogRead(CAP_V_SENSE) / 4;
 
 // write the value to the appropriate byte of the EEPROM.
 // these values will remain there when the board is
 // turned off.
 EEPROM.write(addr, val);  
 addr = addr + 1;
 
 delay(250);
 blink();
 
 
 }
 }
 */
void blink()
{
  digitalWrite(LED2, LED2STATE );
  LED2STATE = ! LED2STATE;

}

void printElect()
{
  Serial.print( analogRead(VOLTAGE_SENSE ) );
  Serial.print( ", " );
  float rawCurrent = analogRead(CURRENT_SENSE );
  rawCurrent -= 512;
  if( rawCurrent < 0 )
    rawCurrent = 0;

  float currentMV = (5000 / 1024) * rawCurrent;
  float currentA = currentMV / 185;
  Serial.println(  currentA );
}

void purge()
{
  Serial.println( "purge" );
  delay( PREPURGE );
  digitalWrite( PURGE, HIGH );
  delay( PURGEDEL);
  digitalWrite( PURGE, LOW );
}

void shortC()
{
  Serial.println( "short" );
  //open circuit 
  digitalWrite( LOAD, LOW );
  delay(DEL);
  //short circuit
  digitalWrite( SHORT, HIGH );
  digitalWrite( LED, HIGH );
  delay(SHORTDEL);
  digitalWrite( LED, LOW );
  //reconnect
  digitalWrite( SHORT, LOW );
  delay( DEL );
  digitalWrite( LOAD, HIGH );

}


