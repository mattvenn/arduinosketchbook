/*
buffer tips:
http://hackaday.com/2015/10/29/embed-with-elliot-going-round-with-circular-buffers/

rs485 tips
http://www.gammon.com.au/forum/?id=11428

crc tips
http://www.leonardomiliani.com/en/2013/un-semplice-crc8-per-arduino/

pwm:
http://playground.arduino.cc/Code/PwmFrequency
https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
Want to increase pwm freq to avoid horrid noise.

 * Pins 5 and 6 are paired on timer0 (also used for delay and millis)
 * Pins 9 and 10 are paired on timer1 (timer1 used for our pid calc)
 * Pins 3 and 11 are paired on timer2 (available for use)

So, I need to switch the interrupt pid timer to timer2 so I can continue using 2&3 for encoder. Then use timer 1 for faster pwm on 9&10.

Then need to check that can do SW serial on other pins (using 8&9 ATM)



*/

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#include <Encoder.h>
#define ENCA 3
#define ENCB 2
#define SerialTxControl 4   //RS485 Direction control
#define REV 5
#define FOR 6
#define TRIG 7 //just for testing
#define SLAVE_TX 8
#define SLAVE_RX 9
#define SSerialTxControl 10   //RS485 Direction control
#define LED 13

Encoder myEnc(ENCA, ENCB);
#include <SoftwareSerial.h>
SoftwareSerial slave_serial(SLAVE_RX, SLAVE_TX); // RX, TX

boolean running = false;
volatile bool calc;
int timer1_counter = 0;
uint8_t last_id = 0;

const float mm_to_pulse = 35.3688;
const int enc_offset = 14851;

//pid globals
int pwm = 128;
unsigned int posref = 0; //servo setpoint in mm
long curpos = 0;
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

//message structures
typedef struct {
    uint8_t command;
    unsigned int lpos;
    unsigned int rpos;
    uint8_t id;
    uint8_t cksum;
} Packet;

typedef struct {
    uint8_t status;
    uint8_t data;
    uint8_t cksum;
} Response;

typedef struct {
    unsigned int pos;
    uint8_t cksum;
} Slave;

//ring buffer for movement commands
#define BUFFER_SIZE 32 //must be a power of 2!
struct Buffer {
	Packet data[BUFFER_SIZE];
	uint8_t newest_index;
	uint8_t oldest_index;
};

struct Buffer buffer = {{}, 0, 0};;

//command definitions
enum BufferStatus {BUFFER_OK, BUFFER_EMPTY, BUFFER_FULL, BAD_CKSUM, MISSING_DATA,BUFFER_LOW, BUFFER_HIGH, BAD_CMD, START, STOP, LOAD, FLUSH, STATUS };

//ring buffer functions
void load(Packet data);
enum BufferStatus bufferWrite(Packet byte);
enum BufferStatus bufferRead(Packet *byte);
enum BufferStatus bufferStatus();

enum BufferStatus bufferStatus()
{
    int space = (buffer.newest_index - buffer.oldest_index + BUFFER_SIZE) % BUFFER_SIZE;
    if(space == 0)
        return BUFFER_EMPTY;
    if(space < BUFFER_SIZE / 2)
        return BUFFER_LOW;
    if(space > 3 * BUFFER_SIZE / 4)
        return BUFFER_HIGH;
    return BUFFER_OK; 
}
    
enum BufferStatus bufferWrite(Packet byte){
	uint8_t next_index = (buffer.newest_index+1) % BUFFER_SIZE;

	if (next_index == buffer.oldest_index){
		return BUFFER_FULL;
	}
	buffer.data[buffer.newest_index] = byte;
	buffer.newest_index = next_index;
	return bufferStatus();
}

enum BufferStatus bufferRead(Packet *byte){
	if (buffer.newest_index == buffer.oldest_index){
		return BUFFER_EMPTY;
	}
	*byte = buffer.data[buffer.oldest_index];
	buffer.oldest_index = (buffer.oldest_index+1) % BUFFER_SIZE;
	return BUFFER_OK;
}

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
    slave_serial.begin(57600); // 115200 too fast for reliable soft serial

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

    pinMode(FOR, OUTPUT);
    digitalWrite(FOR,LOW);
    pinMode(REV, OUTPUT);
    digitalWrite(REV,LOW);
    pinMode(LED, OUTPUT);

    pinMode(SerialTxControl, OUTPUT);  
    pinMode(SSerialTxControl, OUTPUT);  
    digitalWrite(SerialTxControl, RS485Receive);  // Init Transceiver
    digitalWrite(SSerialTxControl, RS485Transmit);  // Init Transceiver

    pinMode(TRIG, OUTPUT);

    b0 = kp+ki+kd;
    b1 = -kp-2*kd;
    b2 = kd;
}


void loop()
{
    enum BufferStatus status;

    if(calc)
    {
        calc = false;
        digitalWrite(LED, HIGH);
        //get next command from buffer
        if(running)
        {
            Packet pos;
            status = bufferRead(&pos);
            if (status != BUFFER_EMPTY)
            {
                posref = pos.lpos * mm_to_pulse;
                send_slave(pos.rpos);
            }
        }

        //pid calculation
        curpos = myEnc.read() + enc_offset;
        xn = float(posref - curpos);
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
    
    // must respond if we get a packet
    if(Serial.available() >= sizeof(Packet))
    {
        Packet data;
        char buf[sizeof(Packet)];
        // do something with status?
        int status = Serial.readBytes(buf, sizeof(Packet));

        //copy buffer to structure
        memcpy(&data, &buf, sizeof(Packet));
        //calculate cksum is ok
        if(data.cksum != CRC8(buf,sizeof(Packet)-1))
        {
            send_response(BAD_CKSUM,0);
            //flush
            while(Serial.available())
                Serial.read();
            return;
        }

        switch(data.command)
        {
            case START:
                running = true;
                send_response(START,0);
                break;
            case STOP:
                running = false;
                send_response(STOP,0);
                break;
            case LOAD:
                //load does send_response
                load(data);
                break;
            case STATUS:
                send_response(bufferStatus(),0);
                break;
            case FLUSH:
                //flush buffer
                last_id = 0;
                buffer.oldest_index = 0;
                buffer.newest_index = 0;
                send_response(bufferStatus(),0);
                break;
            default:
                //shouldn't get here
                send_response(BAD_CMD,0);
        }
    }
}

void send_response(uint8_t status, uint8_t data)
{
    //wait for master to finish transmitting and listen
    delay(10);
    Response resp;
    resp.status = status;
    resp.data = data;

    char buf[sizeof(Response)];
    memcpy(&buf, &resp, sizeof(Response));
    resp.cksum = CRC8(buf,sizeof(Response)-1);

    memcpy(&buf, &resp, sizeof(Response));

    // Enable RS485 Transmit    
    digitalWrite(SerialTxControl, RS485Transmit);  
    delay(1);

    for(int b = 0; b < sizeof(Response); b++)
        Serial.write(buf[b]);

    Serial.flush(); // remove this as it will block?
    delay(1);
    // Disable RS485 Transmit      
    digitalWrite(SerialTxControl, RS485Receive); 
}

void send_slave(unsigned int data)
{
    Slave resp;
    resp.pos = data;
    
    char buf[sizeof(Slave)];
    memcpy(&buf, &resp, sizeof(Slave));
    resp.cksum = CRC8(buf,sizeof(Slave)-1);

    memcpy(&buf, &resp, sizeof(Slave));

    for(int b = 0; b < sizeof(Slave); b++)
        slave_serial.write(buf[b]);

}

// loads a data packet into ring buffer, responds to master
void load(Packet data)
{
    //check id is next in series
    if(data.id != (last_id + 1) % 256)
    {
        send_response(MISSING_DATA, last_id);
        return;
    }
        
    //try to add to buffer
    int status = bufferWrite(data);

    if (status != BUFFER_FULL) 
        //success, so store last id
        last_id = data.id;

    //send response, which will let master know how full the buffer is
    send_response(status, last_id);
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
