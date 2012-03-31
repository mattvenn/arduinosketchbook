/* 
 todo:
 + servo hole fine tuning
 - timings
 - serial comms
 */
#include <EEPROM.h>
#include <MsTimer2.h>
#include <Servo.h> 
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

Servo servo;  // create servo object to control a servo 

#define LED_SERVO 13
#define HEATER 3

#define START 2
#define FAN 11
#define OLDFAN 4
#define LED_FAN 6
#define SWITCH 5
#define SERVO_CLOSED 0

int heatVal = 50; //this is inverted
int heatTime = 5;
int ventOpenDelay = 2;
int ventOpenTime = 4;
int holeSize = 20;
int fanVal;
int rawTemp;



void setup()
{
  pinMode(LED_SERVO, OUTPUT);
  pinMode(LED_FAN, OUTPUT );
  flashLED(); 
  flashLED();
  pinMode(OLDFAN, INPUT );
  // digitalWrite( FAN, HIGH ); //high impedance mode
  pinMode(SWITCH, INPUT );
  digitalWrite( SWITCH, HIGH ); 

  analogWrite( HEATER, 255 );   //inverted
  analogWrite( FAN, 255 ); //inverted
  servo.attach(9);  // attaches the servo on pin 9 to the servo object 
  servo.write(SERVO_CLOSED);
  Serial.begin(9600);
  Serial.println( "scent clock" );

  Serial.println( "loading vals from eeprom...");
  readFromEeprom();
  /* unnecessary RTC stuff
   setSyncProvider(RTC.get);   // the function to get the time from the RTC
   if(timeStatus()!= timeSet) 
   Serial.println("Unable to sync with the RTC");
   else
   Serial.println("RTC has set the system time");      
   */
  // releaseScent();

}

void flashLED()
{
  digitalWrite( LED_SERVO, HIGH);
  delay(100);
  digitalWrite( LED_SERVO, LOW );
}
void readParams()
{
  char command = Serial.read();
  if( command == 'A' )
  {
    delay(100);

    //all these are byte values from 0 to 255
    fanVal = Serial.read();
    holeSize = Serial.read();
    heatVal = Serial.read();
    heatTime = Serial.read();    
    ventOpenDelay = Serial.read();
    ventOpenTime = Serial.read();

    //write them all to eeprom
    writeToEeprom();

    flashLED();    

  }
  else if( command == 'B' )
  {
    printVals();
    flashLED();

  }
  else if( command == 'C' )
  {
    flashLED();
    releaseScent();

  }
  else
  {
    //flush serial port
    while( Serial.available() )
    {
      Serial.read();
    }
  }
}
void loop()
{
  if( digitalRead( SWITCH ) == LOW )
  {
    Serial.println( "switch pressed" );
    releaseScent();
  }
  if( Serial.available() )
  {
    readParams();
  }
}

//broken wrt timings
void releaseScent()
{
  Serial.println( "releasing scent" );
  Serial.print( "turning on heater: " );
  Serial.println( heatVal );
  analogWrite( HEATER, 255 - heatVal );
  int time = 0;
  boolean heatOn = true;
  boolean ventOpen = false;
  while( heatOn || ventOpen )
  {
    if( time == heatTime )
    {
      Serial.println( "turning off heater" );
      analogWrite( HEATER, 255 );
      heatOn = false;
    }
    if( time == ventOpenDelay )
    {
      Serial.print( "opening vent to pos: " );
      digitalWrite( LED_SERVO, HIGH );
      Serial.println( holeSize, DEC );
      servo.write( convertHoleSize(holeSize) );

      if( fanVal != 0 )
      {
        Serial.print( "turning on fan: " );
        Serial.println( fanVal );
        digitalWrite( LED_FAN, HIGH );
        analogWrite( FAN, convertFanVal(fanVal) );
      }
      ventOpen = true;
    }
    if( time == ventOpenDelay + ventOpenTime )
    {    
      Serial.println( "turning off fan" );
      analogWrite( FAN, 255 );
      digitalWrite( LED_FAN, LOW );
      Serial.println( "closing vent" );
      servo.write( SERVO_CLOSED ); 
      digitalWrite( LED_SERVO, LOW );     
      ventOpen = false;
    }
    delay(1000);
    Serial.print( "t=" );
    time ++;
    Serial.println( time, DEC );

  }
  Serial.println( "finished" );
}

int convertFanVal( int fan )
{
  if( fan == 0 )
    return 255;
  return 255 - (fanVal + 155);

}
int convertHoleSize( int holeNum )
{
  if( holeNum == 1 )
    return 25;
  if(holeNum == 2 )
    return 50;
  if(holeNum == 3)
    return 90;
  if(holeNum ==4 )
    return 132;
}
/*  
 int heatKnob = analogRead( HEAT_KNOB );
 unsigned char heatVal = map( heatKnob, 0, 1024, 0, 255 );
 unsigned char servoAngle = map( heatKnob, 0, 1024, 0, 180 );
 analogWrite( HEATER, heatVal );
 servo.write( servoAngle );
 //  Serial.print( "heat: " );
 //  Serial.println( heatKnob, DEC );
 //  delay(100);
 
 */


