/* remember this will run at half speed.
*/
// These constants won't change.  They're used to give names
// to the pins used:
const int nozzleIn = 5;  // Analog input pin that the potentiometer is attached to
const int distanceIn = 4; // Analog output pin that the LED is attached to
const int pwmIn = 3;

int nozzleValue = 0;        // value read from the pot
int distanceValue = 0;
int outputValue = 0;        // value output to the PWM (analog out)
int ledControl = 9;

#define AVG_NUM 20
#define NOZZLE_ON 1000
#define LED_MAX 900

void setup() {
 
  Serial.begin(9600); // as we run at half speed will actually be 2400
  }

void loop() {
  // read the analog in value:
  distanceValue = 0;
 //DON'T need to do it like this. compute a running average.
  for( int i = 0; i < AVG_NUM; i ++ )
  {
    distanceValue  += analogRead(distanceIn);            
    delay( 2 );
  }
  distanceValue /= AVG_NUM;
  nozzleValue = analogRead(nozzleIn);            
  
  int pwmValue = analogRead( pwmIn );
  int led_pwm = map( pwmValue, 0, 1024, 0, 255 );

  //protect infra red led from too much current
  if( led_pwm > LED_MAX ) led_pwm = LED_MAX;
  
  if( nozzleValue < NOZZLE_ON )
  {  
      analogWrite( ledControl, led_pwm );
  }
  else
  {
    analogWrite( ledControl, 0 );
  }
/*  
  Serial.print(nozzleValue);      
  Serial.print(",");
  Serial.print(distanceValue);
  Serial.print( "\n" );   
  */
  char nozByte = map( nozzleValue, 0, 1024, 0, 255 );
  char distByte = map( distanceValue, 0, 1024, 0, 255 );
//  sendData( nozByte, distByte );
sendData( 20, 30 );
}

void sendData( char nozzle, char distance )
{
  char cksum = nozzle + distance;
  //start byte
  Serial.print( 0xAA, BYTE );
  delay( 2 );
  //first data byte
  Serial.print( nozzle, BYTE );
    delay( 2 );
  //second data byte
  Serial.print( distance, BYTE );
    delay( 2 );
  //checksum
  Serial.print( cksum, BYTE );

}
