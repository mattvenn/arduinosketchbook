#include <SoftwareSerial.h>
#include <Servo.h>

typedef struct {
    uint8_t amount;
    uint8_t cksum;
} Can;

#define LED_PIN 13
#define XBEE_RX 10
#define XBEE_TX 11 
#define SERVO 9

Servo servo;

SoftwareSerial xbee_serial(XBEE_RX,XBEE_TX); //pin 5 is RX
void setup()
{
  Serial.begin(115200);
  Serial.println("started");
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  xbee_serial.begin(57600);
  servo.attach(SERVO);
}

void loop()
{
    if(xbee_serial.available() >= sizeof(Can))
    {
        Can data;
        char buf[sizeof(Can)];
        // do something with status?
        int status = xbee_serial.readBytes(buf, sizeof(Can));

        //copy buffer to structure
        memcpy(&data, &buf, sizeof(Can));
        //calculate cksum is ok
        if(data.cksum != CRC8(buf,sizeof(Can)-1))
        {
            //ignore broken packet
            Serial.println("bd ck");
            return;
        }
        servo.write(data.amount);
        if(data.amount > 80)
            digitalWrite(LED_PIN,HIGH);
        else
            digitalWrite(LED_PIN,LOW);
        Serial.println(data.amount);
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
