/* 
todo:
+ servo hole fine tuning
- timings
- serial comms
*/
#include <MsTimer2.h>
#include <Servo.h> 
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

Servo servo;  // create servo object to control a servo 

#define LED 13
#define HEATER 3

#define START 2
#define FAN 4


#define SERVO_CLOSED 0
 
int heatVal = 50; //this is inverted
int heatTime = 5;
int ventOpenDelay = 2;
int ventOpenTime = 4;
int holeSize = 20;
boolean fanState = true;
int rawTemp;


void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH );
  pinMode(FAN, OUTPUT );
  digitalWrite( FAN, HIGH ); //inverted
  
  pinMode(START, INPUT );
  digitalWrite( START, HIGH);
  analogWrite( HEATER, 255 );  
  servo.attach(9);  // attaches the servo on pin 9 to the servo object 
  servo.write(SERVO_CLOSED);
  Serial.begin(9600);
  Serial.println( "scent clock" );
  
   setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");      
  
 // releaseScent();

}

void flashLED()
{
      digitalWrite( LED, LOW );
    delay(100);
    digitalWrite( LED, HIGH );
}
void readParams()
{
   char command = Serial.read();
  if( command == 'A' )
  {
    delay(100);

    char fan = Serial.read();
    fanState = fan == 0 ? false : true;
    holeSize = Serial.read();
    heatVal = Serial.read();
    heatTime = Serial.read();    
    ventOpenDelay = Serial.read();
    ventOpenTime = Serial.read();
    
    flashLED();    

  }
  else if( command == 'B' )
  {
    Serial.println( fanState ? '1': '0' );
    Serial.println( holeSize );
    Serial.println( heatVal );
    Serial.println( heatTime );
    Serial.println( ventOpenDelay );
    Serial.println( ventOpenTime );
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
  if( Serial.available() )
  {
    readParams();
  }
  if( digitalRead( START ) == LOW )
  {
    releaseScent();
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
      Serial.println( holeSize, DEC );
      servo.write( holeSize );
      if( fanState )
      {
        Serial.println( "turning on fan" );
        digitalWrite( FAN, LOW );
      }
      ventOpen = true;
    }
    if( time == ventOpenDelay + ventOpenTime )
    {    
      if( fanState )
      {
        Serial.println( "turning off fan" );
        digitalWrite( FAN, HIGH );
      }
     
      Serial.println( "closing vent" );
      servo.write( SERVO_CLOSED );      
      ventOpen = false;
    }
    delay(1000);
    time ++;
    Serial.print( time, DEC );
    Serial.println( "s" );
  }
  Serial.println( "finished" );
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
  
