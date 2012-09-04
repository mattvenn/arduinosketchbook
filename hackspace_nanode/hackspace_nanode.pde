// Simple demo for feeding some random data to Pachube.
// Based on pachube.pde 2011-07-08 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
// Created by <maniacbug@ymail.com>
//
// See blog post at http://maniacbug.wordpress.com/2011/08/07/nanode/

// This has been tested with EtherCard rev 7752
// Get it from http://jeelabs.net/projects/11/wiki/EtherCard
#include <EtherCard.h>

// change these settings to match your own setup
#define FEED    "46756"
#define APIKEY  "QHcIMwn4vsbSC3kgzClHrh_3XdiSAKw0b1dvY1VBV3JQRT0g"

// On Nanode, this will get the MAC from the 11AA02E48 chip
byte mymac[6];

// Static IP configuration to use if no DHCP found
// Change these to match your site setup
static byte static_ip[] = { 10,0,0,100 };
// gateway ip address
static byte static_gw[] = { 10,0,0,1 };
static byte static_dns[] = { 10,0,0,1 };

char website[] PROGMEM = "api.pachube.com";

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



void setup () {
  Serial.begin(57600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN,LOW);
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
  
  //always fails, so setup static straight away.
  //if (!ether.dhcpSetup("Nanode"))
  {
    printf_P(PSTR("DHCP failed, using static configuration\n\r"));
    ether.staticSetup(static_ip, static_gw);
    ether.copyIp(ether.dnsip, static_dns);
  }

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    printf_P(PSTR("DNS failed\n\r"));
    
  ether.printIp("SRV: ", ether.hisip);
  digitalWrite(LED_PIN,HIGH);
}

void getData()
{
  lightLevel = 1024 - analogRead( LIGHT_PIN ); //to invert it so higher number = more light
  getTemp(); //updates tempStr
}

void loop () {
  ether.packetLoop(ether.packetReceive());

  if (millis() > timer ) {
    getData();
    digitalWrite(LED_PIN,LOW);
  
    timer = millis() + 10000;

    printf_P(PSTR("Sending...\n\r"));
    
    // generate two fake values as payload - by using a separate stash,
    // we can determine the size of the generated message ahead of time
    byte sd = stash.create();
    stash.print("light,");
    stash.println((word) lightLevel);
    stash.print("temp,");
    stash.println(tempStr );
    stash.save();
    
    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
                        "Host: $F" "\r\n"
                        "X-PachubeApiKey: $F" "\r\n"
                        "Content-Length: $D" "\r\n"
                        "\r\n"
                        "$H"),
            website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    ether.tcpSend();


    //ether.browseUrl(PSTR("/foo/"), "bar", website, my_callback);
    digitalWrite(LED_PIN,HIGH);
  }
  
}

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
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
    } else {
      BIT0;
    }
    data <<= 1;
  }
  // MAK
  BIT1;
  // SAK?
  /*bool sak =*/ unio_readBit();
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
    } else {
      BIT1; // MAK
    }
    /*bool sak =*/ unio_readBit();
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
