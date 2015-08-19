#include <ESP8266WiFi.h>

//#define TESTGPIO
//#define TESTWIFI
//#define TESTADC
#define TESTDEEPSLEEP

#define PINS 9
int pins[PINS]={0,2,4,5,12,13,14,15,16};


const char* ssid     = "MRB2015";
const char* password = "9639045620";
//where to send 'hello' to
const char* host = "192.168.1.192";
const char* url = "/";
const int httpPort = 8000;

void setup() 
{
    #ifdef TESTGPIO
    for(int p=0; p<PINS; p++)
        pinMode(pins[p],OUTPUT);
    #endif

    Serial.begin(9600);
    Serial.println("");
    Serial.println("starting");

    #ifdef TESTWIFI
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
    #ifdef TESTGPIO
    for(int p=0; p<PINS; p++)
    {
        Serial.println(p);
        digitalWrite(pins[p],HIGH);
        delay(100);
        digitalWrite(pins[p],LOW);
        delay(100);
    }
    #endif

    #ifdef TESTWIFI
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(host, httpPort)) 
    {
        Serial.println("connection failed");
        return;
    }
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
    #endif

    #ifdef TESTADC
    Serial.println(analogRead(A0));
    delay(1000);
    #endif

    #ifdef TESTDEEPSLEEP
    //gpio16 to reset
    Serial.println("sleeping");
    Serial.flush();
    ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
    delay(100);
    #endif
}
