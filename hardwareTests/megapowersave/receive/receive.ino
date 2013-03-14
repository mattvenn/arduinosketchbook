//this is the low power arduino
/*

    default serial io with arduino is one start bit (0), which we can use to wake.  
  turning off peripherals doesn't seemt o make a difference to wake up time.
 
 9600 baud is 9600 bits per second or 10khz ish
 digitalread/write is 1mhz ish?

  sleep mode: pwr_down
    at 9600 baud, miss first 5 chars of the wakeup byte

    
  are external interrupts faster than pinchange?

  todo:
  
   check external ints are they faster - doesn't work - don't know why.
  look at different sleep modes:
    pwr_down, everything so far means we miss first 5 bits of first byte
    standby, capture the first byte
  hymera serial research: 
    find out if we can miss any part of the first byte? 
    does have start bit?
  brown out disable will save more power
*/
#include <PinChangeInt.h> //see details on this to reduce memory footprint
#include <avr/sleep.h>
#include <avr/power.h>
//#define CAPTURE 
#define PINCHANGE
const int capture = 20;
boolean buff [capture];
byte r;
boolean ready = false;

#define led 13
boolean led_state;
long timer;
void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  Serial.println("starting");
    
}

void attachInt()
{
  #ifdef PINCHANGE
    PCintPort::attachInterrupt(0, &wakeUpNow, FALLING);  // add more attachInterrupt code as required
  #else
  attachInterrupt(0,wakeUpNow, FALLING); // use interrupt 0 (pin 2) and run function
  #endif
}

void detachInt()
{
  #ifdef PINCHANGE
    PCintPort::detachInterrupt(0);
  #else
    detachInterrupt(0); 
  #endif

}

void power_down()
{
  Serial.println("power down");
  delay(50); //allow message to go out
  //power_all_disable();
  
  power_adc_disable();
   power_spi_disable();
   power_timer0_disable();
   power_timer1_disable();
   power_usart0_disable();
   power_timer2_disable();
   power_twi_disable();
   
}


void power_up()
{
  power_all_enable();
 // Serial.begin(9600);
  Serial.println("powered up");
}

void loop()
{
    if(millis()-timer>500)
    {
       timer = millis();
       digitalWrite(led,led_state);
       led_state = !led_state;

    }
 
    if( Serial.available() )
    {
      char c = Serial.read();
      switch( c)
      {
        case 's':
          Serial.println("sleep");
          delay(50);
         // power_down();

          attachInt();
          sei();
          sleep_now();
          
          Serial.println("woke");
          //detachInt();
        //  power_up();
          print_buff();
          break;
        default:
          Serial.print("unknown command:");Serial.println(c);
          break;
      }
    }
}

void print_buff()
{
  for( int i = 0; i < capture; i ++ )
  {
    Serial.print(buff[i]);
    buff[i]=false;
  }
  Serial.println("");
}
