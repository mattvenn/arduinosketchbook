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
  write_log("gsm connected");
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
const int kNetworkTimeout = 30*1000;
const int kNetworkDelay = 1000;


void send_data_arcola(float uptime, float batt, float temp)
{
Serial.println("arcola");
/*
delay(1000);
  write_log("connecting to arcola");
  HttpClient http(client);
  int err = http.get("arduino.cc", "/asciilogo.txt");
//  err = http.get(kHostname, kPath);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
               
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

*/
}
void send_data_xively(float uptime, float batt, float temp)
{
  write_log("connecting to xively");
  datastreams[0].setFloat(uptime);
  datastreams[1].setFloat(batt);
  datastreams[2].setFloat(temp);

  int ret = xivelyclient.put(feed, APIKEY);
  String msg = "xivelyclient.put returned ";
  msg += ret; 
  write_log(msg);
}
