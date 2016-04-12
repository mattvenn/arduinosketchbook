/*
mpd command ref:
https://www.musicpd.org/doc/protocol/command_reference.html
http://www.nerdparadise.com/tech/interview/randomobjectfromstream/
*/

#include <PCD8544.h>
#include <ESP8266WiFi.h>
#include "secrets.h"
IPAddress server(192,168,1,113);
WiFiClient client;

#include <Encoder.h>
Encoder myEnc(4,5); //didn't work with 13, 15, 14, 16

void setup()
{
    Serial.begin(9600);
    Serial.println();
    Serial.println();

    start_wifi();
    randomSeed(ESP.getCycleCount());

    while(true)
        if(connect_mpd())
            break;
    /*
    while(true)
    {
        if(connect_mpd())
        {
            clear_playlist();
            set_vol(50);
            if(load_random_album())
            {
                play();
                Serial.println(get_current_album());
            }
            Serial.println("finished");
            break;
        }
        delay(500);
    }
    */
}

long oldPosition = -999;
void loop()
{
    //nothing
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) 
    {
        if(newPosition > 100)
        {
            newPosition = 100;
            myEnc.write(100);
        }
        if(newPosition < 0)
        {
            myEnc.write(0);
            newPosition = 0;
        }
        oldPosition = newPosition;

        Serial.println(newPosition);
        set_vol(newPosition);
    }
}


bool connect_mpd()
{
    Serial.println("try to connect");
    const int mpdPort = 6600;
    if(!client.connect(server, mpdPort)) 
    {
        Serial.println("couldn't get socket");
        return false;
    }
    boolean conn = false;
    //wait for the ack
    delay(10);
    //get the ack
    while(client.available())
    {
        String line = client.readStringUntil('\n');
        //Serial.print(">"); Serial.println(line);
        if(line.startsWith("OK MPD"))
        {
            Serial.println("connected");
            return true;
        }
    }
    Serial.println("timed out");
    return false;
} 

bool clear_playlist()
{
    //clear playlist
    client.print("clear\n");
    delay(10);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
    return true;
}

bool play()
{
    client.print("play\n");
    delay(10);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
}

bool set_vol(int vol)
{
    client.print("setvol ");
    client.print(vol);
    client.println();
    delay(10);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
}

bool load_random_album()
{
    client.print("list album\n");
    delay(10);

    int albums = 1;
    String choice;
    while(client.available())
    {
        String line = client.readStringUntil('\n');
        if(line == "OK")
        {
            Serial.println("got OK");
            break;
        }
        int ind = line.indexOf(':');
        if(ind == -1)
        {
            Serial.println("unexpected line");
            Serial.println(line);
            return false;
        }
        String album = line.substring(ind + 2);
        album.trim();

        if(album.length() == 0)
            continue;

        albums += 1;
        //Serial.println(album);
        if(random(1, albums) == 1)
            choice = album;
    }
    Serial.print("choice:");
    Serial.println(choice);
    client.print("findadd album ");
    client.print('"');
    client.print(choice);
    client.println('"');
    delay(10);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
    return true;
} 

String get_current_album()
{
    //Serial.print("avail:");
    //Serial.println(client.available());
    String current_album = "no album playing";
    //request current song
    client.print("currentsong\n");
    delay(10);
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available())
    {
        String line = client.readStringUntil('\n');
        if(line == "OK")
        {
            Serial.println("got OK");
            break;
        }
        int ind = line.indexOf(':');
        if(ind == -1)
            break;
        String field = line.substring(0, ind);
        String value = line.substring(ind + 2);
        //Serial.println(ind);
        //Serial.println(field);
        //Serial.println(value);
        //value.trim();
        if(field == "Artist")
            current_album = value + ":";
        if(field == "Album")
            current_album += value;
        //Serial.println(field + "=" + value);
    }
    return current_album;
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
        //update_lcd(0);
    }

    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

