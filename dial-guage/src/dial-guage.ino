#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include "secrets.h"

Ticker t_publish;
boolean publish_now = false;

IPAddress server(192,168,0,10);
WiFiClient wclient;
PubSubClient client(wclient);

const int dial_pin = 5;
const int dial_min = 5;
const int dial_max = 995;
const int dial_min_allow = 30; //it gets stuck at amounts below 30
const int dial_max_allow = dial_max;

char message_buff[100];

//client callback for MQTT subscriptions
void callback(char* topic, byte* payload, unsigned int len) 
{
  Serial.print("topic");
  Serial.println(topic);
  payload[len] = '\0';
  if(strcmp(topic,"/dial/guage") == 0)
  {
      String s = String((char*)payload);
      int value = s.toInt();
      dial(value);
      Serial.println(value);
  }
}

/*
write a number between 0 and 1000 to the dial
handles min and max amounts
prevents dial from getting stuck at min
*/
void dial(int amount)
{
    int out = map(amount,1000,0,dial_min,dial_max);
    if(out > dial_max_allow)
        out = dial_max_allow;
    if(out < dial_min_allow)
        out = dial_min_allow;
    analogWrite(dial_pin,out);
}

void setup() 
{
  WiFi.mode(WIFI_STA);
  analogWriteFreq(50);
  dial(0);
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println();

  //client callback for MQTT subscriptions
  client.setCallback(callback);
  client.setServer(host, 1883);
  //publish uptime every 60 seconds
  t_publish.attach(60, publish);
}

void loop() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    dial(250);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
    WiFi.mode(WIFI_STA);
  }
  else if(WiFi.status() == WL_CONNECTED) 
  {
      //not connected - then connect & subscribe
      if(!client.connected())
      {
          dial(500);
          //don't clean session so queued messages are received
          //if (client.connect(MQTT::Connect("dial").unset_clean_session()))
          if (client.connect("dial"))
          {
            Serial.println("connected to MQTT");
//            client.subscribe("/dial/guage", 2); //qos level 2
            client.subscribe("/dial/guage"); 
            dial(750);
          }
      }
      else if(publish_now)
      {
        Serial.println("publishing uptime to MQTT");
        String pubString = String(millis());
        pubString.toCharArray(message_buff, pubString.length()+1);
        client.publish("/dial/uptime",message_buff);

        publish_now = false;
      }
  }

  delay(500);
  client.loop();
}

void publish()
{
    publish_now = true;
}
