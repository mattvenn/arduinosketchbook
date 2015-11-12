/*
buffer tips:
http://hackaday.com/2015/10/29/embed-with-elliot-going-round-with-circular-buffers/
*/

#include <Encoder.h>
Encoder myEnc(2,3);

#define FOR 5
#define REV 6


int pwm = 128;
unsigned int posref = 0;
// long a1 = 0;
// long a2 = 0;
float b0 = 0;
float b1 = 0;
float b2 = 0;
double yn = 0;
double ynm1 = 0;
// long ynm2 = 0;
float xn = 0;
float xnm1 = 0;
float xnm2 = 0;
float kp = .9;
float ki = 0.000;
float kd = .5;

typedef struct {
    unsigned int lpos;
    unsigned int rpos;
    uint8_t id;
    uint8_t cksum;
} Packet;

#define BUFFER_SIZE 32 //must be a power of 2!

struct Buffer {
	Packet data[BUFFER_SIZE];
	uint8_t newest_index;
	uint8_t oldest_index;
};

struct Buffer buffer = {{}, 0, 0};;

enum BufferStatus {BUFFER_OK, BUFFER_EMPTY, BUFFER_FULL, BAD_CKSUM, MISSING_DATA,BUFFER_LOW, BUFFER_HIGH};

enum BufferStatus bufferWrite(Packet byte);
enum BufferStatus bufferRead(Packet *byte);
enum BufferStatus bufferStatus();

enum BufferStatus bufferStatus()
{
//    int space = (buffer.newest_index - buffer.oldest_index) % BUFFER_SIZE;
    int space = (buffer.newest_index - buffer.oldest_index + BUFFER_SIZE) % BUFFER_SIZE;
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
	return BUFFER_OK;
}

enum BufferStatus bufferRead(Packet *byte){
	if (buffer.newest_index == buffer.oldest_index){
		return BUFFER_EMPTY;
	}
	*byte = buffer.data[buffer.oldest_index];
	buffer.oldest_index = (buffer.oldest_index+1) % BUFFER_SIZE;
	return BUFFER_OK;
}

volatile bool calc;
int timer1_counter = 0;
int packet_len;

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  calc = true;
}

void setup()
{
  Serial.begin(115200);
  //test_space();
  packet_len = sizeof(Packet);

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

  b0 = kp+ki+kd;
  b1 = -kp-2*kd;
  b2 = kd;
}

void loop(){
    enum BufferStatus status;
    static uint8_t last_id;

    if(calc)
    {
        calc = false;
        Packet pos;
        status = bufferRead(&pos);
        if (status == BUFFER_EMPTY) {      
            //underrun
            Serial.write(BUFFER_EMPTY);
        } 
        else
            posref = pos.lpos;
     long newPosition = myEnc.read();
     xn = float(posref - newPosition);
      yn = ynm1 + (b0*xn) + (b1*xnm1) + (b2*xnm2);
      ynm1 = yn;
      if(yn > 127)
      {
        yn = 127;
      }
      if(yn < -128)
      {
        yn = -128;
      }
      
      pwm = 128 + int(yn);   
  
  //write pwm values
  analogWrite(FOR,255-pwm);
  analogWrite(REV,pwm);

  //set previous input and output values
      xnm1 = xn;
      xnm2 = xnm1;
    }
    
    if(Serial.available() >= packet_len)
    {
        Packet data;
        char buf[sizeof(Packet)];
        int status = Serial.readBytes(buf, sizeof(Packet));
        memcpy(&data, &buf, sizeof(Packet));
        /*
        Serial.print(status);
        Serial.print("\n");
        Serial.print(data.lpos);
        Serial.print("\n");
        Serial.print(data.rpos);
        Serial.print("\n");
        Serial.print(data.id);
        Serial.print("\n");
        Serial.print(data.cksum);
        Serial.print("\n");
        return;
        */

        //calculate cksum is ok
        /*
        if(data.cksum != CRC8(&data,packet_len))
        {
            Serial.print(BAD_CKSUM);
            return;
        }
        */

        //check id is next in series
        if(data.id != (last_id + 1) % 256)
        {
            Serial.write(MISSING_DATA);
            Serial.write(last_id);
            return;
        }
            
        //add to buffer
        status = bufferWrite(data);
        if (status == BUFFER_FULL) 
        {      
            Serial.write(BUFFER_FULL);
            Serial.write(last_id);
            return;
        }
        else if(status == BUFFER_OK)
        {
            last_id = data.id;
            status = bufferStatus();
            if(status == BUFFER_HIGH)
                Serial.write(BUFFER_HIGH);
        }
    }
}

/*
byte CRC8(const byte *data, byte len) {
  byte crc = 0x00;
  while (len--) {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}
*/
