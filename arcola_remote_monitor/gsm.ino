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
//XivelyClient xivelyclient(client);   


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


//worked once.
//annoying to do this by hand when we're already including the httpclient library, but I can't work out how it works!
long int global_count = 0;
void send_data_arcola(String ballz)
{

  String send_str = "log100,1,2,3";
  send_str += ",";
  send_str += global_count ++;
  send_str += ",";
  send_str += millis();
  Serial.println(send_str);
  // if there's a successful connection:
  //
  if(client.connect("arcolatheatre.com", 8161))

  {
    Serial.println("connecting...");
    Serial.println(client.connected());
    // send the HTTP PUT request:
    client.println("PUT /recorder.php HTTP/1.1");
    client.println("Host: arcolatheatre.com");


    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:

    client.println(send_str.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.println(send_str);
    //wait for response
    for(int i =0; i < 50; i ++ )
    {
      print_client_msg();
      delay(100);
    }

    Serial.println("done");

  } 
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted
  lastConnectionTime = millis();
}
void send_data_xively(float uptime, float batt, float temp)
{
  /*
  write_log("connecting to xively");
   datastreams[0].setFloat(uptime);
   datastreams[1].setFloat(batt);
   datastreams[2].setFloat(temp);
   
   int ret = xivelyclient.put(feed, APIKEY);
   String msg = "xivelyclient.put returned ";
   msg += ret; 
   write_log(msg);
   */
}


