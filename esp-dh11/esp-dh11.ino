/*
DHT library by Written by ladyada, public domain
git@github.com:adafruit/DHT-sensor-library.git
*/

#include <ESP8266WiFi.h>
#include "secrets.h"

#include "DHT.h"
/*
with ESP8266 avoid using pins 0, 2 and 15 as they are involved with the boot process
https://zoetrope.io/tech-blog/esp8266-bootloader-modes-and-gpio-state-startup
                                    GPIO0   GPIO2   GPIO15
UART Download Mode (Programming)    0       1       0
Flash Startup (Normal)              1       1       0
SD-Card Boot                        0       0       1

additionally, 40ms after startup, GPIO0 line is driven with a signal at around 350 Hz for around 100 ms
*/
#define LEDPIN 2
#define FANPIN 13 
#define DHTPIN 4
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

#define POST_TIME_MS 1000 * 60 * 1 //sample every 1 mins
int wifi_connects = 0;
float set_point = 0;
boolean fan = false;

// state machine
enum states
{
    NOT_CONNECTED,
    SAMPLING,
    CHECK_WIFI,
    POSTING,
};

states state = NOT_CONNECTED;
int last_state = -1;


void setup()
{
    Serial.begin(9600);
    pinMode(LEDPIN, OUTPUT);
    pinMode(FANPIN, OUTPUT);
    Serial.println();
    Serial.println("started");
    dht.begin();
}

void loop()
{
    static unsigned long start_time;
    static int samples = 0;
    static float total_temp = 0;
    static float total_humidity = 0;

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
            state = SAMPLING;
            break;
        }
        case SAMPLING:
        {
            digitalWrite(LEDPIN, HIGH);
            delay(500);
            digitalWrite(LEDPIN, LOW);
            // Reading temperature or humidity takes about 250 milliseconds!
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float humidity = dht.readHumidity();
            float temp = dht.readTemperature();

            // Check if any reads failed and exit early (to try again).
            if (isnan(humidity) || isnan(temp))
            {
                Serial.println("Failed to read from DHT sensor!");
            }
            else
            {
                samples ++;
                total_temp += temp;
                total_humidity += humidity;
            }

            if((millis() - start_time) > POST_TIME_MS)
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
            set_point = map(analogRead(A0), 0, 1024, 30, 80); //map between 30 and 80%
            Serial.print("setpoint = ");
            Serial.println(set_point);

            // simple bang bang controller for the fan
            if(total_humidity/samples  > set_point)
                fan = true;
            else
                fan = false;

            digitalWrite(FANPIN, fan);

            Serial.print("averaging samples:");
            Serial.println(samples);
            post(total_temp/samples, total_humidity/samples, fan, set_point);

            total_temp = 0;
            total_humidity = 0;
            samples = 0;
            start_time = millis();
            state = SAMPLING;
            break;
        }
    }

}

void post(float temp, float humidity, boolean fan, float set_point)
{
    digitalWrite(LEDPIN, HIGH);
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
    url += "&uptime=";
    url += millis();
    url += "&connects=";
    url += wifi_connects;
    url += "&fan=";
    url += fan;
    url += "&setpoint=";
    url += set_point;
    
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
    digitalWrite(LEDPIN, LOW);
}

void start_wifi()
{
    digitalWrite(LEDPIN, HIGH);
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
    digitalWrite(LEDPIN, LOW);
    wifi_connects ++;
}

