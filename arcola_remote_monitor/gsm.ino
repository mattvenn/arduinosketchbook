/*
 GSM Pachube client
 
 This sketch connects an analog sensor to Pachube (http://www.pachube.com)
 using a Telefonica GSM/GPRS shield.
 
 This example has been updated to use version 2.0 of the Pachube.com API. 
 To make it work, create a feed with a datastream, and give it the ID
 sensor1. Or change the code below to match your feed.
 
 Circuit:
 * Analog sensor attached to analog in 0
 * GSM shield attached to an Arduino
 * SIM card with a data plan
 
 created 4 March 2012
 by Tom Igoe
 and adapted for GSM shield by David Del Peral
 
 This code is in the public domain.
 
 http://arduino.cc/en/Tutorial/GSMExamplesPachubeClient
 
 */

// libraries
#include <GSM.h>
#include <Xively.h>
#include <HttpClient.h>

// Pachube Client data
#define APIKEY         "SSQmVnjhxhWGq4hScV8mKo5piZySAKxZZFhEemZSNHZLbz0g"  // replace your pachube api key here
#define FEEDID         127682                     // replace your feed ID
#define USERAGENT      "My Project"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "giffgaff.com"  // replace your GPRS APN
#define GPRS_LOGIN     "giffgaff"     // replace with your GPRS login
#define GPRS_PASSWORD  "password"  // replace with your GPRS password

// initialize the library instance:
GSMClient client;
GPRS gprs;
GSM gsmAccess;

char uptime_stream[] = "uptime";
char batt_stream[] = "batt_voltage";
char temp_stream[] = "temperature";

XivelyDatastream datastreams[] = {
  XivelyDatastream(uptime_stream, strlen(uptime_stream), DATASTREAM_FLOAT),
  XivelyDatastream(batt_stream, strlen(batt_stream), DATASTREAM_FLOAT),
  XivelyDatastream(temp_stream, strlen(temp_stream), DATASTREAM_FLOAT )
  };
  XivelyFeed feed(FEEDID, datastreams, 3);       // Creating the feed, defining two datastreams
XivelyClient xivelyclient(client);   

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);    // numeric IP for api.pachube.com
char server[] = "api.pachube.com";      // name address for pachube API

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to Pachube.com
boolean notConnected = true;

void setup_gsm()
{
  //Serial.println("connecting");
  write_log("gsm setup");
  while (notConnected) 
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY)
    {
      delay(3000);
      if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY)
      {
        notConnected = false;
      }
    }
    else
    {
      write_log("not connected");
      //Serial.println("not connected");
      delay(1000);
    }
  }
}

void close_connection()
{
  write_log("gsm shutdown");

  while(notConnected==false)
  {
    if(gsmAccess.shutdown())
    {
      delay(1000);
      notConnected = true;
    }
    else
    {
      write_log("gsm shutdown wait");
      delay(1000);
    }
  }
  write_log("gsm shutdown ok");

}
//for debugging
void print_client_msg()
{
  while (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
}

void sendData(float uptime, float batt, float temp)
{
  datastreams[0].setFloat(uptime);
  datastreams[1].setFloat(batt);
  datastreams[2].setFloat(temp);

  int ret = xivelyclient.put(feed, APIKEY);
  String msg = "xivelyclient.put returned ";
  msg += ret; 
  write_log(msg);
}
