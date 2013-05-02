#include <SoftwareSerial.h>
SoftwareSerial gsm(2,3);

#define DEBUG_ON
// length for the internal communication buffer
#define COMM_BUF_LEN        200

// some constants for the IsRxFinished() method
#define RX_NOT_STARTED      0
#define RX_ALREADY_STARTED  1

uint16_t start_reception_tmout; // max tmout for starting reception
uint16_t interchar_tmout;       // previous time in msec.
unsigned long prev_time;        // previous time in msec.

byte *p_comm_buf;               // pointer to the communication buffer
byte comm_buf_len;              // num. of characters in the buffer
byte rx_state;                  // internal state of rx state machine    
byte comm_buf[COMM_BUF_LEN+1];  // communication buffer +1 for 0x00 termination

enum rx_state_enum 
{
  RX_NOT_FINISHED = 0,      // not finished yet
  RX_FINISHED,              // finished, some character was received
  RX_FINISHED_STR_RECV,     // finished and expected string received
  RX_FINISHED_STR_NOT_RECV, // finished, but expected string not received
  RX_TMOUT_ERR,             // finished, no character received 
                            // initial communication tmout occurred
  RX_LAST_ITEM
};
enum at_resp_enum 
{
  AT_RESP_ERR_NO_RESP = -1,   // nothing received
  AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
  AT_RESP_OK = 1,             // response_string was included in the response

  AT_RESP_LAST_ITEM
};

void setup()
{
     Serial.begin(9600);
     Serial.println("started");

    pinMode(8,OUTPUT);

    gsm.begin(2400);

    int resp = SendATCmdWaitResp("AT",500,100,"OK",5);
    if( resp == AT_RESP_ERR_NO_RESP )
    {
        Serial.println("no response");
        powerCycle();
    }
    else if( resp == AT_RESP_ERR_DIF_RESP )
    {
        Serial.println("wasn't expecting response");
        powerCycle();
    }
    else
        Serial.println("OK");

}

void powerCycle()
{
    //turn on gsm
    digitalWrite(8,HIGH);
    delay(1200);
    digitalWrite(8,LOW);
}
char SendATCmdWaitResp(char const *AT_cmd_string,
                uint16_t start_comm_tmout, uint16_t max_interchar_tmout,
                char const *response_string,
                byte no_of_attempts)
{
  byte status;
  char ret_val = AT_RESP_ERR_NO_RESP;
  byte i;

  for (i = 0; i < no_of_attempts; i++) {
    // delay 500 msec. before sending next repeated AT command 
    // so if we have no_of_attempts=1 tmout will not occurred
    if (i > 0) delay(500); 

    gsm.println(AT_cmd_string);
    status = WaitResp(start_comm_tmout, max_interchar_tmout); 
    if (status == RX_FINISHED) {
      // something was received but what was received?
      // ---------------------------------------------
      if(IsStringReceived(response_string)) {
        ret_val = AT_RESP_OK;      
        break;  // response is OK => finish
      }
      else ret_val = AT_RESP_ERR_DIF_RESP;
    }
    else {
      // nothing was received
      // --------------------
      ret_val = AT_RESP_ERR_NO_RESP;
    }
    
  }

  return (ret_val);
}

void RxInit(uint16_t start_comm_tmout, uint16_t max_interchar_tmout)
{
  rx_state = RX_NOT_STARTED;
  start_reception_tmout = start_comm_tmout;
  interchar_tmout = max_interchar_tmout;
  prev_time = millis();
  comm_buf[0] = 0x00; // end of string
  p_comm_buf = &comm_buf[0];
  comm_buf_len = 0;
  gsm.flush(); // erase rx circular buffer
}
byte WaitResp(uint16_t start_comm_tmout, uint16_t max_interchar_tmout)
{
  byte status;

  RxInit(start_comm_tmout, max_interchar_tmout); 
  // wait until response is not finished
  do {
    status = IsRxFinished();
  } while (status == RX_NOT_FINISHED);
  return (status);
}

byte IsStringReceived(char const *compare_string)
{
  char *ch;
  byte ret_val = 0;

  if(comm_buf_len) {
	#ifdef DEBUG_ON
		Serial.print("ATT: ");
		Serial.println(compare_string);
		Serial.print("RIC: ");
		Serial.println((char *)comm_buf);
	#endif
    ch = strstr((char *)comm_buf, compare_string);
    if (ch != NULL) {
      ret_val = 1;
	  /*#ifdef DEBUG_PRINT
		DebugPrint("\r\nDEBUG: expected string was received\r\n", 0);
	  #endif
	  */
    }
	else
	{
	  /*#ifdef DEBUG_PRINT
		DebugPrint("\r\nDEBUG: expected string was NOT received\r\n", 0);
	  #endif
	  */
	}
  }

  return (ret_val);
}

byte IsRxFinished(void)
{
  byte num_of_bytes;
  byte ret_val = RX_NOT_FINISHED;  // default not finished

  // Rx state machine
  // ----------------

  if (rx_state == RX_NOT_STARTED) {
    // Reception is not started yet - check tmout
    if (!gsm.available()) {
      // still no character received => check timeout
      if ((unsigned long)(millis() - prev_time) >= start_reception_tmout) {
        // timeout elapsed => GSM module didn't start with response
        // so communication is takes as finished
        comm_buf[comm_buf_len] = 0x00;
        ret_val = RX_TMOUT_ERR;
      }
    }
    else {
      // at least one character received => so init inter-character 
      // counting process again and go to the next state
      prev_time = millis(); // init tmout for inter-character space
      rx_state = RX_ALREADY_STARTED;
    }
  }

  if (rx_state == RX_ALREADY_STARTED) {
    // Reception already started
    // check new received bytes
    // only in case we have place in the buffer
    num_of_bytes = gsm.available();
    // if there are some received bytes postpone the timeout
    if (num_of_bytes) prev_time = millis();
      
    // read all received bytes      
    while (num_of_bytes) {
      num_of_bytes--;
      if (comm_buf_len < COMM_BUF_LEN) {
        // we have still place in the GSM internal comm. buffer =>
        // move available bytes from circular buffer 
        // to the rx buffer
        *p_comm_buf = gsm.read();

        p_comm_buf++;
        comm_buf_len++;
        comm_buf[comm_buf_len] = 0x00;  // and finish currently received characters
                                        // so after each character we have
                                        // valid string finished by the 0x00
      }
      else {
        // comm buffer is full, other incoming characters
        // will be discarded 
        // but despite of we have no place for other characters 
        // we still must to wait until  
        // inter-character tmout is reached
        
        // so just readout character from circular RS232 buffer 
        // to find out when communication id finished(no more characters
        // are received in inter-char timeout)
        gsm.read();
      }
    }

    // finally check the inter-character timeout 
	/*
	#ifdef DEBUG_GSMRX
		
			DebugPrint("\r\nDEBUG: intercharacter", 0);			
<			Serial.print((unsigned long)(millis() - prev_time));	
			DebugPrint("\r\nDEBUG: interchar_tmout\r\n", 0);			
			Serial.print(interchar_tmout);	
			
		
	#endif
	*/
    if ((unsigned long)(millis() - prev_time) >= interchar_tmout) {
      // timeout between received character was reached
      // reception is finished
      // ---------------------------------------------
	  
		/*
	  	#ifdef DEBUG_GSMRX
		
			DebugPrint("\r\nDEBUG: OVER INTER TIMEOUT", 0);					
		#endif
		*/
      comm_buf[comm_buf_len] = 0x00;  // for sure finish string again
                                      // but it is not necessary
      ret_val = RX_FINISHED;
    }
  }
		
	
  return (ret_val);
}
void loop()
{
    if (gsm.available()) 
    {
      Serial.print((char)gsm.read());
      }
      if (Serial.available()) {
          gsm.print((char)Serial.read());
    }
}
