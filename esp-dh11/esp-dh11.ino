/*
DHT library by Written by ladyada, public domain
git@github.com:adafruit/DHT-sensor-library.git
*/

#include <ESP8266WiFi.h>
#include "secrets.h"

#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

#define SAMPLETIME_MS 1000 * 60 * 1 //sample every 1 mins

// state machine
#define NOT_CONNECTED 1
#define WAITING 2
#define CHECK_WIFI 3
#define POSTING 4

int state = NOT_CONNECTED;
int last_state = -1;

void setup()
{
    Serial.begin(9600);
    Serial.println();
    Serial.println();
    dht.begin();
}

void loop()
{
    static unsigned long start_time;

    if(last_state != state)
    {
        last_state = state;
    }

    delay(5);

    switch(state)
    {
        case NOT_CONNECTED:
        {
            start_wifi();
            state = WAITING;
            break;
        }
        case WAITING:
        {
            if((millis() - start_time) > SAMPLETIME_MS)
                state = CHECK_WIFI;
            break;
        }
        case CHECK_WIFI:
        {
            if(WiFi.status() != WL_CONNECTED) 
                state = NOT_CONNECTED;
            else
                state = POSTING;
            break;
        }
        case POSTING:
        {
            post();
            start_time = millis();
            state = WAITING;
            break;
        }
    }

}

void post()
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temp = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C ");
    
    //send the data to sparkfun
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if(!client.connect(host, httpPort)) 
    {
        Serial.println("connection failed");
        return;
    }
    
    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&temp=";
    url += temp;
    url += "&humidity=";
    url += humidity;
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    delay(10);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
  
    Serial.println();
    Serial.println("closing connection");
}

void start_wifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);

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

