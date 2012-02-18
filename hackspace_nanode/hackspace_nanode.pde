/*
 * Arduino + Wicked Node / Reciever Posted to Pachube 
 *      Created on: Aug 10, 2011
 *          Author: Victor Aprea
 *   Documentation: http://wickeddevice.com
 *
 *       Source Revision: 567
 *
 * Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
 *
 */

#include "EtherShield.h"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * The following #defines govern the behavior of the sketch. You can view console outputs using the Serial Monitor set to 2400 baud
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MY_MAC_ADDRESS {0x54,0x55,0x58,0x10,0x00,0x25}               // must be uniquely defined for all Nanodes, e.g. just change the last number
#define USE_DHCP                                                     // comment out this line to use static network parameters
#define PACHUBE_API_KEY "QHcIMwn4vsbSC3kgzClHrh_3XdiSAKw0b1dvY1VBV3JQRT0g" // change this to your API key
#define HTTPFEEDPATH "/v2/feeds/46756.csv"                               // change this to th relative URL of your feed

#ifndef USE_DHCP // then you need to supply static network parameters, only if you are not using DHCP
  #define MY_IP_ADDRESS {192,168,  2, 25}
  #define MY_NET_MASK   {255,255,255,  0}
  #define MY_GATEWAY    {192,168,  2,  1}
  #define MY_DNS_SERVER {192,168,  1,  1}
#endif

// change the template to be consistent with your datastreams: see http://api.pachube.com/v2/
#define FEED_POST_MAX_LENGTH 256
static char feedTemplate[] = "{\"version\":\"1.0.0\",\"datastreams\":[{\"id\":\"sensor1\", \"current_value\":\"%d\"},{\"id\":\"sensor2\",\"current_value\":\"%d\"}]}";
static char feedPost[FEED_POST_MAX_LENGTH] = {0}; // this will hold your filled out template
uint8_t fillOutTemplateWithSensorValues(uint8_t node_id, uint8_t sensorValue1, uint8_t sensorValue2, uint8_t sensorValue3, uint8_t sensorValue4){
  // change this function to be consistent with your feed template, it will be passed the node id and four sensor values by the sketch
  // if you return (1) this the sketch will post the contents of feedPost to Pachube, if you return (0) it will not post to Pachube
  // you may use as much of the passed information as you need to fill out the template
  
  snprintf(feedPost, FEED_POST_MAX_LENGTH, feedTemplate, sensorValue1, sensorValue2); // this simply populates the current_value filed with sensorValue1
  return (1);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * You shouldn't need to make changes below here for configuring the sketch
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// mac and ip (if not using DHCP) have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = MY_MAC_ADDRESS;

// IP address of the host being queried to contact (IP of the first portion of the URL):
static uint8_t websrvip[4] = { 0, 0, 0, 0 }; // resolved through DNS

#ifndef USE_DHCP
// use the provided static parameters
static uint8_t myip[4]      = MY_IP_ADDRESS;
static uint8_t mynetmask[4] = MY_NET_MASK;
static uint8_t gwip[4]      = MY_GATEWAY;
static uint8_t dnsip[4]     = MY_DNS_SERVER;
#else
// these will all be resolved through DHCP
static uint8_t dhcpsvrip[4] = { 0,0,0,0 };    
static uint8_t myip[4]      = { 0,0,0,0 };
static uint8_t mynetmask[4] = { 0,0,0,0 };
static uint8_t gwip[4]      = { 0,0,0,0 };
static uint8_t dnsip[4]     = { 0,0,0,0 };
#endif

// global string buffer for hostname message:
#define FEEDHOSTNAME "api.pachube.com\r\nX-PachubeApiKey: " PACHUBE_API_KEY
#define FEEDWEBSERVER_VHOST "api.pachube.com"
static char hoststr[150] = FEEDWEBSERVER_VHOST;

#define BUFFER_SIZE 550
static uint8_t buf[BUFFER_SIZE+1];

EtherShield es=EtherShield();
//WickedReceiver receiver;

#define LIGHT_PIN A0
#define LED_PIN 6
int lightLevel = 10;
int temp = 10;

char str[50];
char fstr[10];
char tempStr[20];

#include <TimedAction.h>
TimedAction ActionUploadData= TimedAction( 10000, uploadData);


  
void setup(){
  Serial.begin(9600);
  setupTherm();
  Serial.println("hackspace nanode");

  // Initialise SPI interface
  es.ES_enc28j60SpiInit();

  // initialize ENC28J60
  es.ES_enc28j60Init(mymac, 8);

#ifdef USE_DHCP
  acquireIPAddress();
#endif

  printNetworkParameters();

  //init the ethernet/ip layer:
  es.ES_init_ip_arp_udp_tcp(mymac,myip, 80);

  // init the web client:
  es.ES_client_set_gwip(gwip);  // e.g internal IP of dsl router
  es.ES_dnslkup_set_dnsip(dnsip); // generally same IP as router
  
  Serial.println("Awaiting Client Gateway");
  while(es.ES_client_waiting_gw()){
    int plen = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);
    es.ES_packetloop_icmp_tcp(buf,plen);    
  }
  Serial.println("Client Gateway Complete, Resolving Host");

  resolveHost(hoststr, websrvip);
  Serial.print("Resolved host: ");
  Serial.print(hoststr);
/*
  websrvip[0] = 10;
  websrvip[1] = 0;
  websrvip[2] = 0;
  websrvip[3] = 2;
  */
  Serial.print(" to IP: ");
  printIP(websrvip);
  Serial.println();
  
  es.ES_client_set_wwwip(websrvip);
}

void loop(){
  int plen = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);
  es.ES_packetloop_icmp_tcp(buf,plen);
    ActionUploadData.check();
}

void formatString()
{
  
    // Convert int/double to string, add it to main string, add csv commas
    // dtostrf - converts a double to a string!
    // strcat  - adds a string to another string
    // strcpy  - copies a string
    strcpy(str,"light,");
    
    dtostrf(lightLevel,0,3,fstr); 
    strcat(str,fstr);
    strcat(str,"\ntemp,");
    
    strcat(str,tempStr);

    
}

//callback after the http push
void browserresult_callback(uint8_t statuscode,uint16_t datapos) 
{
  // statuscode=0 means a good webpage was received, with http code 200 OK
  // statuscode=1 an http error was received
  // statuscode=2 means the other side in not a web server and in this case datapos is also zero

  if (statuscode==0)
  {
    Serial.println( "pachube returned OK" );
  }  
  else if( statuscode==1)
  {  
    Serial.println( "pachube error" );
  }
  else
  {
    Serial.print( "callback with unknown status: " );
    Serial.println( statuscode );
  }
}

void getData()
{
  lightLevel = 1024 - analogRead( LIGHT_PIN ); //to invert it so higher number = more light
  getTemp(); //updates tempStr
}
void uploadData()
{
    digitalWrite( LED_PIN, LOW );
    //get the data
    getData();
    
   formatString();
 Serial.println( "sending " );
 Serial.println( str );
      es.ES_client_http_post(PSTR(HTTPFEEDPATH),PSTR(FEEDWEBSERVER_VHOST),PSTR(FEEDHOSTNAME), PSTR("PUT "), str, &browserresult_callback);    
    
  
    digitalWrite( LED_PIN, HIGH );  
}

#ifdef USE_DHCP
void acquireIPAddress(){
  uint16_t dat_p;
  long lastDhcpRequest = millis();
  uint8_t dhcpState = 0;
  Serial.println("Sending initial DHCP Discover");
  es.ES_dhcp_start( buf, mymac, myip, mynetmask,gwip, dnsip, dhcpsvrip );

  while(1) {
    // handle ping and wait for a tcp packet
    int plen = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);

    dat_p=es.ES_packetloop_icmp_tcp(buf,plen);
    //    dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));
    if(dat_p==0) {
      int retstat = es.ES_check_for_dhcp_answer( buf, plen);
      dhcpState = es.ES_dhcp_state();
      // we are idle here
      if( dhcpState != DHCP_STATE_OK ) {
        if (millis() > (lastDhcpRequest + 10000L) ){
          lastDhcpRequest = millis();
          // send dhcp
          Serial.println("Sending DHCP Discover");
          es.ES_dhcp_start( buf, mymac, myip, mynetmask,gwip, dnsip, dhcpsvrip );
        }
      } 
      else {
        return;        
      }
    }
  }   
}
#endif

// hostName is an input parameter, ipAddress is an outputParame
void resolveHost(char *hostName, uint8_t *ipAddress){
  es.ES_dnslkup_request(buf, (uint8_t*)hostName );
  while(1){
    int plen = es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf);
    es.ES_packetloop_icmp_tcp(buf,plen);   
    if(es.ES_udp_client_check_for_dns_answer(buf, plen)) {
      uint8_t *websrvipptr = es.ES_dnslkup_getip();
      for(int on=0; on <4; on++ ) {
     //   Serial.println( *websrvipptr, DEC );
        ipAddress[on] = *websrvipptr++;
  
      }     
      return;
    }    
  }
}  



// Output a ip address from buffer from startByte
void printIP( uint8_t *buf ) {
  for( int i = 0; i < 4; i++ ) {
    Serial.print( buf[i], DEC );
    if( i<3 )
      Serial.print( "." );
  }
}

void printNetworkParameters(){
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
}


