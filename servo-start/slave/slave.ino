typedef struct {
    uint8_t command;
    unsigned int pos;
    uint8_t cksum;
} Slave;

enum SlaveCommand {SLV_LOAD, SLV_SET_POS};


#define RS485Transmit    HIGH
#define RS485Receive     LOW

#include "buttons.h"
#include "utils.h"
#include "pindefs.h"
#include <Encoder.h>
Encoder myEnc(ENCA,ENCB);
#include <SoftwareSerial.h>
SoftwareSerial master_serial(SS_RX, SS_TX); // RX, TX

const float mm_to_pulse = 35.3688;

volatile bool calc;
volatile byte timer2_overflows = 0;
uint8_t timer2_overflow = 0;
uint8_t timer2_remainder = 0;
int timer1_counter = 0;

//pid globals
int pwm = 128;
#define MAX_POSREF 65535
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
//interrupt service routine 
ISR(TIMER2_OVF_vect)        
{
    timer2_overflows ++;
    if(timer2_overflows == timer2_overflow)
    {
        //preload timer
        TCNT2 = timer2_remainder;
    }
    if(timer2_overflows > timer2_overflow)
    {
        timer2_overflows = 0;
        calc = true;
        TCNT2 = 0;
    }
}

void setup()
{
    buttons_setup();
    Serial.begin(115200);
    master_serial.begin(57600); // 115200 too fast for reliable soft serial
    pinMode(LED, OUTPUT);

    pinMode(SSerialTxControl, OUTPUT);  
    digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver

    pinMode(FOR, OUTPUT);
    digitalWrite(FOR,LOW);
    pinMode(REV, OUTPUT);
    digitalWrite(REV,LOW);

    // timer 2 setup
    TCCR2A = 0;
    TCCR1B = 0;

    // set timer prescalar to 1024
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    // turn on interrupt overflow
    TIMSK2 |= (1 << TOIE2);

    // preload timer2 (8 bits)
    timer2_overflow = 1;
    timer2_remainder = 256 - 56;
    TCNT2 = 0;

    // set pwm frequency on pins 9&10 (timer1) to 31250Hz
    TCCR1B = TCCR1B & 0b11111000 | 0x01;


    b0 = kp+ki+kd;
    b1 = -kp-2*kd;
    b2 = kd;

    // turn on interrupts
    interrupts();
}

int bad_cksum = 0;
int ok = 0;

void loop()
{
    switch(buttons_check())
    {
        case IN:
            if(posref < MAX_POSREF)
                posref ++;
            delay(1);
            break;
        case OUT:
            if(posref > 0)
                posref --;
            delay(1);
            break;
        case HOME:
            while(buttons_check() != LIMIT)
                drive(HOME_PWM);
            drive(0);
            posref = 0;
            myEnc.write(0);
            break;
    }
    if(calc)
    {
        calc = false;
        digitalWrite(LED,HIGH);

        //pid calculation
        long newPosition = myEnc.read();
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
        digitalWrite(LED,LOW);
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
        switch(data.command)
        {
            case SLV_LOAD:
                Serial.print("loaded:");
                Serial.println(data.pos);
                posref = data.pos * mm_to_pulse;
                break;
            case SLV_SET_POS:
                Serial.print("setpos:");
                Serial.println(data.pos);
                posref = data.pos * mm_to_pulse;
                myEnc.write(posref);
                break;
        }
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
