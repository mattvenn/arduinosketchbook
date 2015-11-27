typedef struct {
    unsigned int pos;
    uint8_t cksum;
} Slave;

#define LED 13
#define FOR 5 
#define REV 11 

#include <Encoder.h>
Encoder myEnc(2,3);
#include <SoftwareSerial.h>
SoftwareSerial master_serial(8, 9); // RX, TX

const float mm_to_pulse = 35.3688;
const int enc_offset = 14851;

volatile bool calc;
int timer1_counter = 0;

//pid globals
int pwm = 128;
unsigned int posref = 0;
float b0 = 0;
float b1 = 0;
float b2 = 0;
double yn = 0;
double ynm1 = 0;
float xn = 0;
float xnm1 = 0;
float xnm2 = 0;
float kp = .45;
float ki = 0.000;
float kd = .25;

//interrupt service routine 
ISR(TIMER1_OVF_vect)        
{
    //flash light
    //sbi(PORTD,TRIG);
    //preload timer
    TCNT1 = timer1_counter;   
    calc = true;
    //cbi(PORTD,TRIG);
}

void setup()
{
    Serial.begin(115200);
    master_serial.begin(57600); // 115200 too fast for reliable soft serial
    pinMode(LED, OUTPUT);

    pinMode(FOR, OUTPUT);
    digitalWrite(FOR,LOW);
    pinMode(REV, OUTPUT);
    digitalWrite(REV,LOW);

    TCCR1A = 0;
    TCCR1B = 0;

    // Set timer1_counter to the correct value for our interrupt interval
    //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
    timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
    //timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz

    TCNT1 = timer1_counter;   // preload timer
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    interrupts();             // enable all interrupts

    b0 = kp+ki+kd;
    b1 = -kp-2*kd;
    b2 = kd;
}

int bad_cksum = 0;
int ok = 0;

void loop()
{
    if(calc)
    {
        calc = false;
        digitalWrite(LED, HIGH);

        //pid calculation
        long newPosition = myEnc.read() + enc_offset;
        xn = float(posref - newPosition);
        yn = ynm1 + (b0*xn) + (b1*xnm1) + (b2*xnm2);
        ynm1 = yn;

        //limit
        if(yn > 127)
            yn = 127;
        if(yn < -128)
            yn = -128;

        pwm = 128 + int(yn);   

        //write pwm values
        analogWrite(FOR,255-pwm);
        analogWrite(REV,pwm);

        //set previous input and output values
        xnm1 = xn;
        xnm2 = xnm1;
        digitalWrite(LED, LOW);
    }
    if(master_serial.available() >= sizeof(Slave))
    {
        Slave data;
        char buf[sizeof(Slave)];
        // do something with status?
        int status = master_serial.readBytes(buf, sizeof(Slave));

        //copy buffer to structure
        memcpy(&data, &buf, sizeof(Slave));
        //calculate cksum is ok
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
        posref = data.pos * mm_to_pulse;
    }
    if(Serial.available())
    {
        char cmd = Serial.read();
        switch(cmd)
        {
            case 'a':
                Serial.println(ok);
                Serial.println(bad_cksum);
                break;
            case 'b':
                ok = 0;
                bad_cksum = 0;
                break;
        }
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
