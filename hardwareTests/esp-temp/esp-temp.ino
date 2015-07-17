#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

#define LED 0
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
float temp;
long int interval = 1000 * 60 * 5;

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup()
{
    pinMode(LED, OUTPUT);

    // start serial port
    Serial.begin(9600);
    Serial.println("Temperature Sensor");

    // Start up the library
    sensors.begin();

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
}

void loop()
{
    // call sensors.requestTemperatures() to issue a global temperature 
    // request to all devices on the bus
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");

    Serial.print("Temperature for the device 1 (index 0) is: ");
    temp = sensors.getTempCByIndex(0);
    Serial.println(temp);  
    //send it
    send(temp);

    digitalWrite(LED, HIGH);
    delay(interval);
    digitalWrite(LED, LOW);
}

//send the data to sparkfun
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
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
}
