/*
 * WiiChuckDemo -- 
 *
 * 2008 Tod E. Kurt, http://thingm.com/
 *
 */

#include <Wire.h>
#include "/home/matthew/sketchbook/nunchuck/nunchuck_funcs.h"

int loop_cnt=0;

byte accx,accy,zbut,cbut,joyx,joyy;
int ledPin = 13;


void setup()
{
    Serial.begin(57600);
    nunchuck_setpowerpins();
    nunchuck_init(); // send the initilization handshake
    
    Serial.print("WiiChuckDemo ready\n");
}

void loop()
{
    if( loop_cnt > 100 ) { // every 100 msecs get new data
        loop_cnt = 0;

        nunchuck_get_data();

        accx  = nunchuck_accelx(); // ranges from approx 70 - 182
        accy  = nunchuck_accely(); // ranges from approx 65 - 173
        zbut = nunchuck_zbutton();
        cbut = nunchuck_cbutton(); 
        joyx = nunchuck_joyx();
        joyy = nunchuck_joyy();
            
        Serial.print("accx: "); Serial.print((byte)accx,DEC);
        Serial.print("\taccy: "); Serial.print((byte)accy,DEC);
        Serial.print("\tzbut: "); Serial.print((byte)zbut,DEC);
        Serial.print("\tcbut: "); Serial.print((byte)cbut,DEC);
        Serial.print("\tjoyx: "); Serial.print((byte)joyx,DEC);
        Serial.print("\tjoyy: "); Serial.println((byte)joyy,DEC);
    }
    loop_cnt++;
    delay(1);
}

