/* 
Matt Venn's home energy hub. 2011

TODO:
+send gas and elec to robot
-daily kwh
-start accounting for lost data

gets energy info from a wireless energy monitor via Xbee.
sends data to pachube and to polargraph energy monitor
http://www.mattvenn.net/2011/09/19/polargraph-energy-monitoring/

Thanks to:

EtherShield library by: Andrew D Lindsay http://blog.thiseldo.co.uk
RTC stuff from <jcw@equi4.com> http://opensource.org/licenses/mit-license.php
ntp stuff from thiseldo.co.uk : https://gist.github.com/1338239
running on a nanode: Projects: Nanode.eu and OpenEnergyMonitor.org

Licence: GPL GNU v3

Notes:

Had to edit ethershield/ip_config.h to #define NTP_client 1

*/

#include <Wire.h>
#include <EtherShield.h>
#include <TimedAction.h>
//#include <RTClib.h>
#include <NewSoftSerial.h>
#include <NanodeMAC.h>
//RTC_DS1307 RTC;
EtherShield es=EtherShield();
//globals
int minutes = -1;
char str[40];
char fstr[10];
boolean dataReady=false;
double irms, gasPulses, temp, battv;
double sumGasWS = 0, sumElecWS = 0; //counters for energy, will become the totals
double gasKWHH = 0, elecKWHH = 0; //totals for the last hour
double gasKWHD = 0, elecKWHD = 0; //totals for last day

float elecW = 0, elecWS = 0, gasWS = 0; //instantaneous energy values for gas and electricity
double sumEnergyWS; //for the robot
double lastReading = 0; //milliseconds of last reading

//int batteryLevel;

//pin defs
#define LED_PIN 6
#define XBEE_TX 5
#define CC_RX 4

//timed actions
TimedAction ActionUpdateTotals = TimedAction( 1000, updateTotals );

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Serial.println("gas and electric monitor via nanode to pachube");
//  xbeeSetup();
  currentCostSetup();
  //pin setups
  pinMode( LED_PIN, OUTPUT );
  digitalWrite( LED_PIN, HIGH );

  //setup network
  Serial.println( "setting up network" );
  startNetwork();
  setupDHCP();
}

void loop()
{
  //  while( es.ES_dhcp_state() == DHCP_STATE_OK ) { no point in this because it never changes state
  checkNetwork(); //need to call this often. Why? I think because otherwise we'll lose buffered data
  readCurrentCost();
  ActionUpdateTotals.check();
  if( dataReady ) //this flag set if we got energy data via xbee
  {
    dataReady = false;
    
//    doPowerCalculations();
    
    digitalWrite( LED_PIN, LOW );
    sendToPachube();
    digitalWrite( LED_PIN, HIGH );
  }
}

