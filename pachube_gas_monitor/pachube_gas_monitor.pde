#include <TimedAction.h>

//--------------------------------------------------------
//   EtherShield example: simpleClient Pachube
//
//   simple client code layer:
//
// - ethernet_setup(mac,ip,gateway,server,port)
// - ethernet_ready() - check this before sending
//
// - ethernet_setup_dhcp(mac,serverip,port)
// - ethernet_ready_dhcp() - check this before sending
//
// - ethernet_setup_dhcp_dns(mac,domainname,port)
// - ethernet_ready_dhcp_dns() - check this before sending
//
//   Posting data within request body:
// - ethernet_send_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "),str);
// 
//   Sending data in the URL
// - ethernet_send_url(PSTR(HOST),PSTR(API),str);
//
//   EtherShield library by: Andrew D Lindsay
//   http://blog.thiseldo.co.uk
//
//   Example by Trystan Lea, building on Andrew D Lindsay's examples
//
//   Projects: Nanode.eu and OpenEnergyMonitor.org
//   Licence: GPL GNU v3
//--------------------------------------------------------

#include <EtherShield.h>

byte mac[6] =     { 0x54,0x55,0x38,0x12,0x01,0x23};
byte ip[4] =      {192,168,0,100};
byte gateway[4] = {192,168,0,1};
byte server[4] =  {173,203,98,29};


int batteryLevel;
int minutes;
boolean gasPulse = false;

#define PACHUBE_VHOST "www.pachube.com"
#define PACHUBEAPIURL "/api/28462.csv"
#define PACHUBEAPIKEY "X-PachubeApiKey: ZxBqcZRDClLxco2ZUbeat1D6x7pfOL5Jhmo60Ies2TU"

#define LED_PIN 6

TimedAction ActionCheckXbeeData = TimedAction( 200, checkXbeeData);
//#define PACHUBEAPIKEY "www.pachube.com\r\nX-PachubeApiKey: xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" 
//#define PACHUBEAPIURL "/api/0000.csv"

unsigned long lastupdate;
boolean trigger = false;

char str[50];
char fstr[10];
boolean dataReady=false;


double irms, gas, temp, battv;

void setup()
{
  Serial.begin(57600);
  Serial.println("gas and electric monitor via nanode to pachube");
  xbeeSetup();
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, HIGH );
  ethernet_setup(mac,ip,gateway,server,80,8); // Last two: PORT and SPI PIN: 8 for Nanode, 10 for nuelectronics
}

void loop()
{
  ActionCheckXbeeData.check();
  
  if( dataReady )
  {
    formatString();
  }

  //----------------------------------------
  // 2) Send the data
  //----------------------------------------
  if (ethernet_ready() && dataReady)
  {
    digitalWrite( LED_PIN, LOW );
    Serial.print( "pushing: " );
    Serial.println( str );
    ethernet_send_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "),str);
    Serial.println("sent"); 
    Serial.print( "sending to energy robot:" );
    Serial.print( 
    
    dataReady = false;
    digitalWrite( LED_PIN, HIGH );
  }
}

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

}

