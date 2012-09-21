// Simple demo for feeding some random data to Pachube.
// Based on pachube.pde 2011-07-08 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// Created by <maniacbug@ymail.com>
//
// See blog post at http://maniacbug.wordpress.com/2011/08/07/nanode/

// This has been tested with EtherCard rev 7752
// Get it from http://jeelabs.net/projects/11/wiki/EtherCard
#include <EtherCard.h>
#include <JeeLib.h>

// change these settings to match your own setup
#define FEED    "46756"
#define APIKEY  "QHcIMwn4vsbSC3kgzClHrh_3XdiSAKw0b1dvY1VBV3JQRT0g"

// On Nanode, this will get the MAC from the 11AA02E48 chip
byte mymac[6];

// Static IP configuration to use if no DHCP found
// Change these to match your site setup
static byte static_ip[] = { 
  192,168,0,200 };
// gateway ip address
static byte static_gw[] = { 
  192,168,0,1 };
static byte static_dns[] = { 
  192,168,0,1 };

char cosmURL[] PROGMEM = "api.pachube.com";
char robotURL[] PROGMEM = "mattvenn.net";

byte cosmIP[4];
byte robotIP[4];

byte Ethernet::buffer[500];
uint32_t timer;
Stash stash;

void printf_begin(void);
void read_MAC(byte*);

#define LIGHT_PIN A0
#define LED_PIN 6
int lightLevel = 10;
int temp = 10;
char tempStr[20];
boolean gotAck = true;
//robot command def
typedef struct {
  // byte index;
  char command;
  unsigned int arg1;
  unsigned int arg2;

}
Payload;
#define PARSE
#define MAXCOMMANDQUEUE 10
Payload payload[MAXCOMMANDQUEUE];
boolean readyToSend = false;
MilliTimer testTimer;
int commands = 0; // number of commands successfully parsed.
int commandIndex = 0; //which command we're on at the mo

#define MEMTEST
boolean uploadData = true;
boolean updateRobot = true;
unsigned long sentData;
MilliTimer getDataTimer, uploadTimer;

void setup () {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,LOW);

  Serial.println( F("initialising radio") );
  delay(100);
  rf12_initialize(2, RF12_433MHZ,212);
  Serial.println( F("rf12 setup done") );

  setupTherm();
  printf_begin();
  printf_P(PSTR("\nEtherCard/examples/nanode_pachube\n\r"));

  // Fetch the MAC address -- Nanode-specific
  read_MAC(mymac);

  printf_P(PSTR("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n\r"),
  mymac[0],
  mymac[1],
  mymac[2],
  mymac[3],
  mymac[4],
  mymac[5]
    );

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    printf_P(PSTR( "Failed to access Ethernet controller\n\r"));

  //always failed on the netgear. works on the dlink
  if (!ether.dhcpSetup("Nanode"))
  {
    printf_P(PSTR("DHCP failed, using static configuration\n\r"));
    ether.staticSetup(static_ip, static_gw);
    ether.copyIp(ether.dnsip, static_dns);
  }

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  //do the lookups
  if (!ether.dnsLookup(robotURL))
    printf_P(PSTR("DNS failed\n\r"));
  ether.copyIp(robotIP,ether.hisip);
  ether.printIp("robot ip: ", robotIP);
  if (!ether.dnsLookup(cosmURL))
    printf_P(PSTR("DNS failed\n\r"));
  ether.copyIp(cosmIP,ether.hisip);
  ether.printIp("cosm ip: ", cosmIP);


  digitalWrite(LED_PIN,HIGH);
}
int turn= 0;
void getData()
{
  lightLevel = 1024 - analogRead( LIGHT_PIN ); //to invert it so higher number = more light
  getTemp(); //updates tempStr
}
static byte session;


void loop () {
  //poll ethernet
  ether.packetLoop(ether.packetReceive());
  //poll radio
  doRadio();


#ifdef PARSE
  if( commands )
  {
    //wait for an ack, or timeout and resend after 500ms
    if( gotAck == true  || ( millis() - sentData > 500 ))
    {
      if( commandIndex >= commands )
      {
        commands = 0;
        commandIndex = 0;
        Serial.println( F( "finished stack" ) );
      }
      else
      {
        readyToSend = true;
      }
    }
  }
#endif

  if( getDataTimer.poll(10000) )
  {

#ifdef MEMTEST
    Serial.print( "mem: " );
    Serial.println( freeMemory() );
#endif

    if( turn ++ % 2 == 0 )
    {
      printf_P(PSTR("getting robot data\n"));

      digitalWrite(LED_PIN,LOW);

      timer = millis() + 2000;
      ether.hisport = 10002;
      ether.copyIp(ether.hisip,robotIP);
      //  ether.printIp("robot ip: ", ether.hisip);

      ether.browseUrl(PSTR("/"), "", robotURL, my_callback);
      digitalWrite(LED_PIN,HIGH);
    }
    else
    {
      printf_P(PSTR("pushing to cosm\n"));

      getData();

      //http://blog.cuyahoga.co.uk/2012/05/theres-something-wrong-with-my-stash/
      if (stash.freeCount() <= 52) 
      {
        Stash::initMap(56);
      }

      byte sd = stash.create();

      stash.print("light,");
      stash.println((word) lightLevel);
      stash.print("temp,");
      stash.println(tempStr );
      stash.print("stash,");
      stash.println( stash.freeCount() );
      stash.save();
      Serial.print( "stash mem:" ); 
      Serial.println( stash.freeCount() );

      // generate the header with payload - note that the stash size is used,
      // and that a "stash descriptor" is passed in as argument using "$H"
      ether.hisport = 80;
      ether.copyIp(ether.hisip,cosmIP);
      //  ether.printIp("cosm ip: ", ether.hisip);

      Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
        "Host: $F" "\r\n"
        "X-PachubeApiKey: $F" "\r\n"
        "Content-Length: $D" "\r\n"
        "\r\n"
        "$H"),
      cosmURL, PSTR(FEED), cosmURL, PSTR(APIKEY), stash.size(), sd);

      // send the packet - this also releases all stash buffers once done
      session = ether.tcpSend();
    }
  }

  const char * reply  = ether.tcpReply(session);
  {
    if( reply != 0)
    {
      Serial.print( "got cosm rep:" );
      Serial.println( strlen( reply ));
      //Serial.println( reply );
    }
  }

}

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
 // Serial.println("callback");
  //  Serial.println( off );
  //  Serial.println( len );
  int offset = stripHeaders(off);
  commands = parse(offset);
  Serial.print( "parsed: ");
  Serial.println( commands );
  if( commands )
  {
    for( int i =0 ; i < commands ; i ++ )
    {
      Serial.print( i );
      Serial.print( ":" );
      Serial.print( payload[i].command );
      Serial.print( payload[i].arg1 );
      Serial.print( "," );
      Serial.println( payload[i].arg2 );


    }
    Serial.println(F("trashing old commands"));
    commandIndex = 0;
  }
}



int serial_putc( char c, FILE * ) 
{
  Serial.write( c );

  return c;
} 

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

// Nanode_MAC
// Rufus Cable, June 2011 (threebytesfull)

// Sample code to read the MAC address from the 11AA02E48 on the
// back of the Nanode V5 board.

// This code is hacky and basic - it doesn't check for bus errors
// and will probably fail horribly if it's interrupted. It's best
// run in setup() - fetch the MAC address once and keep it. After
// the address is fetched, it puts the chip back in standby mode
// in which it apparently only consumes 1uA.

// Feel free to reuse this code - suggestions for improvement are
// welcome! :)

// http://ww1.microchip.com/downloads/en/DeviceDoc/DS-22067H.pdf
// http://ww1.microchip.com/downloads/en/devicedoc/22122a.pdf 

// Nanode has UNI/O SCIO on PD7 

#define D7_ON  _BV(7) 
#define D7_OFF (~D7_ON)

#define SCIO_HIGH PORTD |= D7_ON
#define SCIO_LOW  PORTD &= D7_OFF

#define SCIO_OUTPUT DDRD |= D7_ON
#define SCIO_INPUT  DDRD &= D7_OFF

#define SCIO_READ ((PIND & D7_ON) != 0)

#define WAIT_QUARTER_BIT delayMicroseconds(9);
#define WAIT_HALF_BIT delayMicroseconds(20);

#define NOP PORTD &= 0xff

// Fixed Timings
// standby pulse time (600us+)
#define UNIO_TSTBY_US 600
// start header setup time (10us+)
#define UNIO_TSS_US 10
// start header low pulse (5us+)
#define UNIO_THDR_US 6

// SCIO Manipulation macros
#define BIT0 SCIO_HIGH;WAIT_HALF_BIT;SCIO_LOW;WAIT_HALF_BIT;
#define BIT1 SCIO_LOW;WAIT_HALF_BIT;SCIO_HIGH;WAIT_HALF_BIT;

// 11AA02E48 defines
#define DEVICE_ADDRESS 0xA0
#define READ_INSTRUCTION 0x03

// Where on the chip is the MAC address located?
#define CHIP_ADDRESS 0xFA

inline bool unio_readBit()
{
  SCIO_INPUT;
  WAIT_QUARTER_BIT;
  bool value1 = SCIO_READ;
  WAIT_HALF_BIT;
  bool value2 = SCIO_READ;
  WAIT_QUARTER_BIT;
  return (value2 && !value1);
}

void unio_standby() {

  SCIO_OUTPUT;
  SCIO_HIGH;
  delayMicroseconds(UNIO_TSTBY_US);
}

void unio_sendByte(byte data) {

  SCIO_OUTPUT;
  for (int i=0; i<8; i++) {
    if (data & 0x80) {
      BIT1;
    } 
    else {
      BIT0;
    }
    data <<= 1;
  }
  // MAK
  BIT1;
  // SAK?
  /*bool sak =*/  unio_readBit();
}

void unio_readBytes(byte *addr, int length) {
  for (int i=0; i<length; i++) {

    byte data = 0;
    for (int b=0; b<8; b++) {
      data = (data << 1) | (unio_readBit() ? 1 : 0);
    }
    SCIO_OUTPUT;
    if (i==length-1) {
      BIT0; // NoMAK
    } 
    else {
      BIT1; // MAK
    }
    /*bool sak =*/    unio_readBit();
    addr[i] = data;
  }
}

void unio_start_header() {
  SCIO_LOW;
  delayMicroseconds(UNIO_THDR_US);
  unio_sendByte(B01010101);
}

void read_MAC(byte* mac_address) {

  // no interrupts!
  cli();

  // standby
  unio_standby();

  // start header
  unio_start_header();

  unio_sendByte(DEVICE_ADDRESS);
  unio_sendByte(READ_INSTRUCTION);
  unio_sendByte(CHIP_ADDRESS >> 8);
  unio_sendByte(CHIP_ADDRESS & 0xff);

  // read 6 bytes
  unio_readBytes(mac_address, 6);

  // back to standby
  unio_standby();

  // interrupts ok now
  sei();
}


