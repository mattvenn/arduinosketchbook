
#include <NewSoftSerial.h>
#include <TimedAction.h>
#include "Emon.h"    //Load the library
#include <avr/sleep.h>
EnergyMonitor emon;  //Create an instance


volatile boolean f_wdt=0;
volatile boolean f_gas=0;
volatile unsigned long last_interrupt_time = 0;
int gas;
int battV;
int temp;

//this initializes a TimedAction class that will change the state of an LED every second.
//TimedAction timedAction = TimedAction(100,reprog);
NewSoftSerial xbee(11, 10);
#define BATT_PIN A0
#define CURRENT_PIN A1
#define TEMP_PIN A2
#define SLEEP_PIN 9
#define LED_PIN 7
#define GAS_PULSE_PIN 2
void setup()  
{
  Serial.begin(57600);
  Serial.println( "starting" );
  pinMode( SLEEP_PIN, OUTPUT );
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, HIGH );

  // set the data rate for the NewSoftSerial port
  xbee.begin(9600);
  emon.setPins(A5,CURRENT_PIN); //v,I                                //Energy monitor analog pins
  emon.calibration( 1.116111611, 0.128401361, 2.3);  //Energy monitor calibration


  //interrupt sleep
  pinMode(GAS_PULSE_PIN, INPUT);
  attachInterrupt(0, wakeUpNow, LOW);

  setup_watchdog(9);
}



// Main loop
//----------------------------------------------------------------------------
void loop()
{
  if( f_wdt == 1 ) // just woken from watchdog
  {
    digitalWrite(LED_PIN, HIGH );
    f_wdt = 0;
    Serial.println( "doing calcs" );
    emon.calc(20,2000);              //Energy Monitor calc function
    battV = analogRead( BATT_PIN );
    temp = analogRead( TEMP_PIN );

    Serial.println( "sending data" );

    sendData();
    gas = 0;
    digitalWrite(LED_PIN, LOW );
  }
  if( f_gas ) //just got woken by a gas pulse
  {
    f_gas = 0;
    gas = 1;
  }

  delay(100);
  sleepNow();
}  

void sendData()
{
  Serial.print( "Irms:");
  Serial.println(emon.Irms, DEC);
  Serial.print( "gas pulses:");
  Serial.println(gas, DEC);
  Serial.print( "battv:");
  Serial.println(battV, DEC);
  Serial.print( "temp:");
  Serial.println(temp, DEC);


  digitalWrite( SLEEP_PIN, LOW ); //disable sleep
  delay( 100 ); //wait for it to wake up
  digitalWrite(LED_PIN, LOW );

  //  xbee.print( "Irms:");
  xbee.print(emon.Irms, DEC);
  //  xbee.print( "gas pulses:");
  xbee.print(",");
  xbee.print(gas, DEC);
  xbee.print(",");
  // xbee.print( "battv:");
  xbee.print(battV, DEC);
  xbee.print(",");
  //xbee.print( "temp:");
  xbee.println(temp, DEC);


  digitalWrite( SLEEP_PIN, HIGH ); //disable sleep





}



