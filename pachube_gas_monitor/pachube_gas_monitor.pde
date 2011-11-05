//   EtherShield library by: Andrew D Lindsay
//   http://blog.thiseldo.co.uk
//
// RTC stuff from // 2010-02-04 <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
//
// ntp stuff from thiseldo.co.uk : https://gist.github.com/1338239
//   Projects: Nanode.eu and OpenEnergyMonitor.org
//   Licence: GPL GNU v3
//--------------------------------------------------------
#include <Wire.h>
#include <EtherShield.h>
#include <TimedAction.h>
#include <RTClib.h>
#include <NewSoftSerial.h>
RTC_DS1307 RTC;
EtherShield es=EtherShield();
#define NANODE 1
#ifdef NANODE
#include <NanodeMAC.h>
#endif

#ifdef NANODE
static uint8_t mymac[6] = { 0,0,0,0,0,0 };
#else
static uint8_t mymac[6] = {
  0x54,0x55,0x58,0x10,0x00,0x25};
#endif

#ifdef NANODE
NanodeMAC mac( mymac );
#endif

#define DEBUG 1 //print lots of stuff

#define BUFFER_SIZE 500
static uint8_t buf[BUFFER_SIZE+1];
uint16_t dat_p;
int plen = 0;
  char dstr[4];
int port;

// IP and netmask allocated by DHCP
static uint8_t myip[4] = { 0,0,0,0 };
static uint8_t mynetmask[4] = { 0,0,0,0 };
static uint8_t gwip[4] = { 0,0,0,0 };
static uint8_t dnsip[4] = { 0,0,0,0 };
static uint8_t dhcpsvrip[4] = { 0,0,0,0 };

// IP address of the host being queried to contact (IP of the first portion of the URL):
static uint8_t websrvip[4] = { 0, 0, 0, 0 };


#define SECS_YR_1900_2000  (3155673600UL)

#define NUM_TIMESERVERS 5
int currentTimeserver = 0;
uint32_t lastUpdate = 0;
// From http://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
byte timeServer[][4] = { 
  { 64, 90, 182, 55    }  , // nist1-ny.ustiming.org
  { 66, 27, 60, 10   }  , // ntp2d.mcc.ac.uk
  { 130, 88, 200, 4    }  , // ntp2c.mcc.ac.uk
  { 31, 193, 9, 10     }  , // clock02.mnuk01.burstnet.eu 
  { 82, 68, 133, 225   }   // ntp0.borg-collective.org.uk
 };

 uint16_t clientPort = 123;
 
/*
byte mac[6] =     { 0x54,0x55,0x38,0x12,0x01,0x23};
byte ip[4] =      {192,168,0,100};
byte gateway[4] = {192,168,0,1};

*/
byte server[4] =  {173,203,98,29};
int batteryLevel;
int minutes = -1;
boolean gasPulse = false;
uint32_t time;

#define PACHUBE_VHOST "www.pachube.com"
#define PACHUBEAPIURL "/api/28462.csv"
#define PACHUBEAPIKEY "X-PachubeApiKey: ZxBqcZRDClLxco2ZUbeat1D6x7pfOL5Jhmo60Ies2TU"

#define LED_PIN 6
#define XBEE_RX 4
#define XBEE_TX 5

TimedAction ActionCheckXbeeData = TimedAction( 200, checkXbeeData);
TimedAction ActionSendNTP = TimedAction( 10000, ntpRequest); //once a minute
TimedAction ActionPrintRTC = TimedAction( 1000, printRTCTime);
TimedAction ActionCheckNTP = TimedAction( 100, checkNTP );
//#define PACHUBEAPIKEY "www.pachube.com\r\nX-PachubeApiKey: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" 
//#define PACHUBEAPIURL "/api/0000.csv"

char str[50];
char fstr[10];
boolean dataReady=false;

double irms, gas, temp, battv, power;

void setup()
{
  Wire.begin();
  RTC.begin();
  Serial.begin(9600);
  #ifdef DEBUG
  Serial.println("gas and electric monitor via nanode to pachube");
  printRTCTime();
  #endif
  xbeeSetup();

lastUpdate = millis();
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, HIGH );

  es.ES_enc28j60SpiInit();
  // initialize enc28j60
#ifdef NANODE
  es.ES_enc28j60Init(mymac,8);
#else
  es.ES_enc28j60Init(mymac);
#endif
  Serial.print( "ENC28J60 version " );
  Serial.println( es.ES_enc28j60Revision(), HEX);
  if( es.ES_enc28j60Revision() <= 0 ) {
    Serial.println( "Failed to access ENC28J60");

    while(1);    // Just loop here
  }
  

//  ethernet_setup(mac,ip,gateway,server,80,8); // Last two: PORT and SPI PIN: 8 for Nanode, 10 for nuelectronics
  #ifdef DEBUG
  Serial.println( "net setup" );
  #endif
}

void loop()
{

  
   if( es.allocateIPAddress(buf, BUFFER_SIZE, mymac, 80, myip, mynetmask, gwip, dnsip, dhcpsvrip ) > 0 ) {
#ifdef DEBUG
    // Display the results:
    Serial.print( "My IP: " );
    printIP( myip );
    Serial.println();

    Serial.print( "Netmask: " );
    printIP( mynetmask );
    Serial.println();

    Serial.print( "DNS IP: " );
    printIP( dnsip );
    Serial.println();

    Serial.print( "GW IP: " );
    printIP( gwip );
    Serial.println();

 // init the ethernet/ip layer
  es.ES_init_ip_arp_udp_tcp(mymac,myip, 80);
  es.ES_client_set_gwip(gwip);  // e.g internal IP of dsl router
  es.ES_client_tcp_set_serverip(server);
#endif    

  } 
  else {
    // Failed, do something else....
#ifdef DEBUG
    Serial.println("Failed to get IP Address");
#endif    
  }

  // Main processing loop now we have our addresses

  while( es.ES_dhcp_state() == DHCP_STATE_OK ) {
  
    // handle ping and wait for a tcp packet
   //?
   
   checkNTP(); //need to call this often?
   ActionSendNTP.check();
  ActionPrintRTC.check(); //this also updates the global minutes variable
  ActionCheckXbeeData.check();
  
  if( dataReady )
  {
    digitalWrite( LED_PIN, LOW );

    sendRobotData();
    dataReady = false;

    {
      #ifdef DEBUG
      Serial.print( "pushing to pachube: " );
      formatString();
      Serial.println( str );
      #endif
     // ethernet_send_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "),str);
       es.ES_client_http_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "),str,&browserresult_callback);
    } 

      digitalWrite( LED_PIN, HIGH );
  }
  }

}

//format the energy numbers into a string to send to pachube
void formatString()
{
  
    // Convert int/double to string, add it to main string, add csv commas
    // dtostrf - converts a double to a string!
    // strcat  - adds a string to another string
    // strcpy  - copies a string
    strcpy(str,"");
    
    dtostrf(battv,0,3,fstr); 
    strcat(str,fstr);
    strcat(str,",");
    
    dtostrf(gas,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");    
    
    dtostrf(temp,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");    
    
    dtostrf(irms,0,3,fstr);
    strcat(str,fstr);
    strcat(str,",");
    
    dtostrf(power,0,3,fstr);
    strcat(str,fstr);
    
}

