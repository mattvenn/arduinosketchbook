// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;

void setupRTC () {
    Wire.begin();
    RTC.begin();
    RTC.adjust(DateTime(__DATE__, __TIME__));

  if (! RTC.isrunning()) {
//    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

unsigned long getUnixSecs()
{
    DateTime now = RTC.now();
    return now.unixtime();
}

void printSerialDigits(byte digits){
  // utility function for digital clock display: prints colon and leading 0
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);   
}
void printDate()
{
    DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    printSerialDigits(now.month());
    Serial.print('/');
    printSerialDigits(now.day());
    Serial.print(' ');
    printSerialDigits(now.hour());
    Serial.print(':');
    printSerialDigits(now.minute());
    Serial.print(':');
    printSerialDigits(now.second());
    Serial.println();
}
