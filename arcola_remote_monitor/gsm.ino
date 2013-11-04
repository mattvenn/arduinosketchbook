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
char memory_stream[] = "memory";


XivelyDatastream datastreams[] = {
  XivelyDatastream(uptime_stream, strlen(uptime_stream), DATASTREAM_FLOAT),
  XivelyDatastream(batt_stream, strlen(batt_stream), DATASTREAM_FLOAT),
  XivelyDatastream(temp_stream, strlen(temp_stream), DATASTREAM_FLOAT),
  XivelyDatastream(memory_stream, strlen(memory_stream), DATASTREAM_FLOAT )

  };
  
XivelyFeed feed(FEEDID, datastreams, 3);       // Creating the feed, defining two datastreams
XivelyClient xivelyclient(client);   


unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 5*1000;  //delay between updates to Pachube.com
boolean notConnected = true;

void setup_gsm()
{
  notConnected = true;
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
  write_log("gsm connected");
}

void close_connection()
{
  write_log("gsm shutdown");
  Serial.println(client.connected());

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

//uses httpclient to do a put to the arcola website
void send_data_arcola(String data)
{
  //don't understand how this works - needs changing
  String send_str = "log100,";
  send_str += data;
  
  write_log("connecting to arcola");

  HttpClient http(client);
  http.beginRequest();
  int ret = http.put("arcolatheatre.com", 8161, "/recorder.php");
  if (ret == 0)
  {
    http.sendHeader("Content-Length", send_str.length());
    http.print(send_str);
    http.endRequest();

    ret = http.responseStatusCode();
    String msg = "arcola put returned ";
    msg += ret; 
    write_log(msg);
    
    http.stop();
  } 
  else
  {
    Serial.println("connection failed");
  }
}
void send_data_xively(float uptime, float batt, float temp, int memory)
{
   write_log("connecting to xively");
   datastreams[0].setFloat(uptime);
   datastreams[1].setFloat(batt);
   datastreams[2].setFloat(temp);
   datastreams[3].setFloat((float)memory);
   
   int ret = xivelyclient.put(feed, APIKEY);
   String msg = "xively put returned ";
   msg += ret; 
   write_log(msg);
   
}



