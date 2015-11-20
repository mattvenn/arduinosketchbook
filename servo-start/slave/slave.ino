typedef struct {
    unsigned int pos;
    uint8_t cksum;
} Slave;

#define LED 13
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 9); // RX, TX

void setup()
{
    Serial.begin(19200);
    mySerial.begin(19200);
    pinMode(LED, OUTPUT);
}

int bad_cksum = 0;
int ok = 0;

void loop()
{
    if(mySerial.available() >= sizeof(Slave))
    {
        digitalWrite(LED, HIGH);
        Slave data;
        char buf[sizeof(Slave)];
        // do something with status?
        int status = mySerial.readBytes(buf, sizeof(Slave));

        //copy buffer to structure
        memcpy(&data, &buf, sizeof(Slave));
        //calculate cksum is ok
        digitalWrite(LED, LOW);
        if(data.cksum != CRC8(buf,sizeof(Slave)-1))
        {
            //ignore broken packet
            bad_cksum ++;
            //Serial.println("bad cksum");
            return;
        }
        ok ++;
        //Serial.println("ok!");
        //set the servo position
        //...
    }
    if(Serial.available())
    {
        Serial.read();
        Serial.println(ok);
        Serial.println(bad_cksum);
    }
}

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
