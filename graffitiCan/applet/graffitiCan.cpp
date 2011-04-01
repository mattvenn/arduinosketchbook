#include <VirtualWire.h>
/* remember this will run at half speed.
*/
// These constants won't change.  They're used to give names
// to the pins used:
#include "WProgram.h"
void setup();
void loop();
void sendData( char nozzle, char distance );
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
 pinMode( 1, OUTPUT );
  vw_set_tx_pin(1);
  vw_setup(2000); // Bits per sec
pinMode( 13, OUTPUT );

  }

void loop() {
  // read the analog in value:
digitalWrite( 13, LOW );
  delay(200 );
digitalWrite( 13, HIGH );  
  delay(200 );
  
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
//      analogWrite( ledControl, led_pwm );
  }
  else
  {
 //   analogWrite( ledControl, 0 );
  }


  char nozByte = map( nozzleValue, 0, 1024, 0, 255 );
  char distByte = map( distanceValue, 0, 1024, 0, 255 );
//  sendData( nozByte, distByte );


sendData( 20, 30 );
}

void sendData( char nozzle, char distance )
{
  char buf[2];
  buf[0] = nozzle;
  buf[1] = distance;
  vw_send((uint8_t *)buf, strlen(buf));

}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

