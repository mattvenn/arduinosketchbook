// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;

void setup_rtc () {
    Wire.begin();
    RTC.begin();
    RTC.adjust(DateTime(__DATE__, __TIME__));
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

String get_time()
{
  DateTime now = RTC.now();
     String time = 
       String(now.year()) +
       "/" +
       String(now.month()) +
       "/" +
       String(now.day()) +
       " " +
       String(now.hour()) +
       ":" + 
       String(now.minute()) +
       ":" +
       String(now.second());
       
return time;
  
}
void print_time () {
    DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
   
}
