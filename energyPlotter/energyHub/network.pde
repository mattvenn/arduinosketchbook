byte pachubeServer[4] = { 173,203,98,29}; //DNS doesn't work yet
#define PACHUBE_VHOST "api.pachube.com"
#include "keys.h"


static uint8_t mymac[6] = { 0,0,0,0,0,0 };
NanodeMAC mac( mymac );

#define BUFFER_SIZE 500
static uint8_t buf[BUFFER_SIZE+1];
uint16_t dat_p;

// IP and netmask allocated by DHCP
static uint8_t myip[4] = { 0,0,0,0 };
static uint8_t mynetmask[4] = { 0,0,0,0 };
static uint8_t gwip[4] = { 0,0,0,0 };
static uint8_t dnsip[4] = { 0,0,0,0 };
static uint8_t dhcpsvrip[4] = { 0,0,0,0 };

  
//send data to pachube
void sendToPachube()
{
  formatString();
  
  Serial.print( "pushing to pachube: " );
  Serial.println( str );

  es.ES_client_http_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "),str,&browserresult_callback);
}

//callback after the http push
void browserresult_callback(uint8_t statuscode,uint16_t datapos) 
{
  // statuscode=0 means a good webpage was received, with http code 200 OK
  // statuscode=1 an http error was received
  // statuscode=2 means the other side in not a web server and in this case datapos is also zero

  if (statuscode==0)
    Serial.println( "pachube returned OK" );
  if( statuscode==1)
    Serial.println( "pachube error" );
}

//call regularly to keep network going
void checkNetwork()
{
  dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));
}

//start network
void startNetwork()
{
  es.ES_enc28j60SpiInit();
  // initialize enc28j60
  es.ES_enc28j60Init(mymac,8);
  Serial.print( "ENC28J60 version " );
  Serial.println( es.ES_enc28j60Revision(), HEX);
  if( es.ES_enc28j60Revision() <= 0 ) {
    Serial.println( "Failed to access ENC28J60");
    while(1);    // Just loop here
  }
  Serial.println( "network started" );

}

//get IP etc
void setupDHCP()
{
  if( es.allocateIPAddress(buf, BUFFER_SIZE, mymac, 80, myip, mynetmask, gwip, dnsip, dhcpsvrip ) > 0 ) {
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
    es.ES_client_tcp_set_serverip(pachubeServer);    

  } 
  else {
    // Failed, do something else....
    Serial.println("Failed to get IP Address");
  }
  Serial.println( "network setup" );
}

//print IP addresses
void printIP( uint8_t *buf ) {
  for( int i = 0; i < 4; i++ ) {
    Serial.print( buf[i], DEC );
    if( i<3 )
      Serial.print( "." );
  }
}
