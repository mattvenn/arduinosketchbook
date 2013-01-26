#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 RTC;
void setupRTC()
{
    Wire.begin();
    RTC.begin();

    while (! RTC.isrunning()) 
    {
        Serial.println("RTC is NOT running!");
        // following line sets the RTC to the date & time this sketch was compiled
        RTC.adjust(DateTime(__DATE__, __TIME__));
        delay(100);
    }

}
void updateTime(int hour,int min)
{
    RTC.adjust(DateTime(12,1,1,hour,min,55));
}

struct time getTime()
{
    DateTime now = RTC.now();
    if( now.hour() == 165)    
    {
        Serial.println("RTC error!");
        return {-1,-1};
    }
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

    return {now.hour(),now.minute()};
}
