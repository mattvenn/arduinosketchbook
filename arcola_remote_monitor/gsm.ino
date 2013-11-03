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

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);    // numeric IP for api.pachube.com
char server[] = "api.pachube.com";      // name address for pachube API

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to Pachube.com
boolean notConnected = true;

void setup_gsm(){
     Serial.println("connecting");

  while (notConnected) {
    digitalWrite(gsm_power,HIGH);
    if(gsmAccess.begin(PINNUMBER)==GSM_READY){
      delay(3000);
      if(gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY){
        notConnected = false;
      
      }
    }
    else{
            Serial.println("Not connected");

      delay(1000);
    }
  }
}

void close_connection(){
              Serial.println("shutdown ");

 // while(notConnected==false){
    gsmAccess.shutdown();
      delay(1000);
      digitalWrite(gsm_power,LOW);
      notConnected = true;
/*    
    }
    else{
                    Serial.println("shutdown wait ");

      delay(1000);
    }
  }*/
                Serial.println("shutdown ok ");

}
void print_client_msg()
{
  while (client.available())
  {
     char c = client.read();
     Serial.print(c);
  }
}

void power_up()
{
    delay(1000);
    digitalWrite(gsm_power,LOW);
}
void power_down()
{
 
    client.stop();
    digitalWrite(gsm_power,HIGH);
    delay(5000);
    digitalWrite(gsm_power,LOW);
  
}

  
 
 

/*
  This method makes a HTTP connection to the server.
*/
void sendData(int thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println("sending...");
    
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.pachube.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(thisData);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
    // here's the actual content of the PUT request:
    client.print("sensor1,");
    client.println(thisData);
    client.stop();
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

/*
  This method calculates the number of digits in the
  sensor reading.  Since each digit of the ASCII decimal
  representation is a byte, the number of digits equals
  the number of bytes.
*/
int getLength(int someValue)
{
  // there's at least one byte:
  int digits = 1;
  
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0)
  {
    dividend = dividend /10;
    digits++;
  }
  
  // return the number of digits:
  return digits;
}

