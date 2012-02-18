#include <VirtualWire.h>
// remember this will run at half speed. as we use the internal RC clock
// fuse settings: lfuse:w:0xf2:m


//globals
int nozzleValue = 0;        // value read from the pot
int distanceValue = 0;

//pin defs
#define IR_LED  5 //pwm pin for ir LED
#define STATUS_LED 8 //13 //status led
#define TX_PIN 1 //radio tx pin
#define NOZZ_ADC 5 // Analog input pin that the potentiometer is attached to
#define DIST_ADC 4 // Analog output pin that the LED is attached to

//noz ranges
#define NOZZLE_ON 1020
#define MIN_NOZ_ADC 700 //614
#define MAX_NOZ_ADC 1024 //1024
#define MIN_NOZ_OUT 0
#define MAX_NOZ_OUT 255

void setup() {
  pinMode( TX_PIN, OUTPUT );
  vw_set_tx_pin(TX_PIN);
  vw_setup(8000); // Bits per sec (half this though as running at 8mhz)
  pinMode( STATUS_LED, OUTPUT );
  pinMode( IR_LED, OUTPUT );
  digitalWrite( STATUS_LED, LOW );
  }

void loop() {
  //turn off IR LED to make the reading of the distance value
  digitalWrite( IR_LED, LOW );
  
  // read the analog in values
  nozzleValue = analogRead(NOZZ_ADC);            
  distanceValue = analogRead( DIST_ADC );

  if( nozzleValue < NOZZLE_ON )
  {  
        digitalWrite( IR_LED, HIGH );
  }

  //interestd in 5v - 3v (614 -> 1024 @5v)
  nozzleValue = constrain( nozzleValue, MIN_NOZ_ADC, MAX_NOZ_ADC );
  char nozByte = map( nozzleValue, MIN_NOZ_ADC, MAX_NOZ_ADC, MIN_NOZ_OUT, MAX_NOZ_OUT );

  //interested in 0 - 3v ( 0 -> 614 @5v)
  distanceValue = constrain( distanceValue, 200, 800 );
  char distByte = map( distanceValue, 200, 800, 0, 255 );

  digitalWrite( STATUS_LED, LOW );
  sendData( nozByte, distByte );
  digitalWrite( STATUS_LED, HIGH );
}

void sendData( char nozzle, char distance )
{
  vw_wait_tx();
  char buf[2];
  if( nozzle == 0 ) nozzle = 1;
  if( distance == 0 ) distance = 1;
  buf[0] = nozzle;
  buf[1] = distance;
  vw_send((uint8_t *)buf, strlen(buf));
}
