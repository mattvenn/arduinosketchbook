#include <OneWire.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

#define LED 0
#define ONE_WIRE_BUS 2
#define WIFI

OneWire oneWire(ONE_WIRE_BUS);
long int interval = 1000 * 60; // * 5;
//long int interval = 1000; // * 60; // * 5;
float temp;

void setup()
{
    pinMode(LED, OUTPUT);
    Serial.begin(9600);
    Serial.println("Wifi Temperature Sensor");

    #ifdef WIFI
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    #endif

}

void loop()
{
    // get temp
    while( ! get_temp() )
        delay(1000);

    Serial.println(temp);  

    #ifdef WIFI
    // send it
    send(temp);
    #endif

    // wait
    digitalWrite(LED, HIGH);
    delay(interval);
    digitalWrite(LED, LOW);
}

// send the data to sparkfun
void send(float value)
{
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }
    
    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&temp=";
    url += value;
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");

    // wait for response
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
}
