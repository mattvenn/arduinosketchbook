#include "utils.h"
#include "pindefs.h"

static const float FILTER = 0.9; //coefficient for LPF on current sense
float filteredRawCurrent = 1024/2; //0A with no load

//crc from Dallas Semi
byte CRC8(char *data, byte len) 
{
    byte crc = 0x00;
    while (len--)
    {
        byte extract = *data++;
        for (byte tempI = 8; tempI; tempI--) 
        {
            byte sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if(sum) 
            {
                crc ^= 0x8C;
            }
            extract >>= 1;
        }
    }
    return crc;
}

float read_current()
{
    //100 times average of current.
    filteredRawCurrent = filteredRawCurrent * FILTER  + ( 1 - FILTER ) * analogRead(CURRENT);
    float currentMV = (5000.0 / 1024.0 ) * filteredRawCurrent;
    //current sense chip is powered by arduino supply
    float current = ( currentMV - 2500 ) / 185; //185mv per amp
    return current;
}

void drive(int yn)
{
    //limit
    if(yn > 127)
        yn = 127;
    if(yn < -128)
        yn = -128;

    int pwm = 128 + int(yn);   
    analogWrite(FOR,255-pwm);
    analogWrite(REV,pwm);
}
