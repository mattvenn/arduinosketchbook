#include <MsTimer2.h>
#include <Servo.h> 
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

Servo servo;  // create servo object to control a servo 

#define LED 13
#define HEATER 4

#define START 2
#define FAN 3


#define SERVO_OPEN 180
#define SERVO_CLOSED 0

int heatVal = 50; //this is inverted
int heatA = 10000;
int gateA = 2000;
int gateD = 4000;
boolean fanState = true;
int rawTemp;


void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH );
  pinMode(FAN, OUTPUT );
  digitalWrite( FAN, LOW );
  
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
  
  releaseScent();

}


void loop()
{
  if( digitalRead( START ) == LOW )
  {
    releaseScent();
  }

  delay( 200 );
     digitalClockDisplay();  
}

void releaseScent()
{
  Serial.println( "releasing scent" );
  Serial.println( "turning on heater" );
  analogWrite( HEATER, heatVal );

  delay( gateA );

  Serial.println( "opening gate" );
  servo.write( SERVO_OPEN );
  digitalWrite( FAN, fanState );
  //hack, ensure heatA > gateA
  delay( heatA - gateA );
  
  Serial.println( "turning off heater" );
  analogWrite( HEATER, 255 );
  
  delay( gateD );
  
  Serial.println( "closing gate" );
  digitalWrite( FAN, LOW );
  servo.write( SERVO_CLOSED );
  
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
  
