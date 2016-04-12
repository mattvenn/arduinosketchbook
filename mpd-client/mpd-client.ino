/*
mpd command ref:
https://www.musicpd.org/doc/protocol/command_reference.html
http://www.nerdparadise.com/tech/interview/randomobjectfromstream/

PCB notes
* added pullup/down resisters as per instructable
http://www.instructables.com/id/ESP8266-Using-GPIO0-GPIO2-as-inputs/?ALLSTEPS
* worth noting correct direction of encoder
* how to avoid lcd pot

*/

#include <PCD8544.h>
#include <ESP8266WiFi.h>
#include "secrets.h"
IPAddress server(192,168,1,113);
WiFiClient client;

#include <Encoder.h>
int knob_pos = 0;
int old_knob_pos = 0;
int knob_max = 0;
int last_volume = 0;
int last_menu = 0;
unsigned long state_timer = 0;

Encoder knob(4,5); //didn't work with 13, 15, 14, 16

enum states
{
    WIFI,
    MPD_CONNECT,
    MENU_START,
    MENU_WAIT,
    MENU_UPDATE,
    VOLUME_START,
    VOLUME_WAIT,
    VOLUME_UPDATE,
    RANDOM,
    FIP,
};

enum menus
{
    MENU_VOLUME,
    MENU_RANDOM,
    MENU_FIP,
    MENU_LAST_ITEM, // MUST BE THE LAST ITEM //
};

int state = WIFI;
int next_state = 0;

#define BUTTON 0

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 15, 13,12,14,16);

void setup()
{
    pinMode(BUTTON, INPUT); //has external pullup

    Serial.begin(9600);
    Serial.println();
    Serial.println();

    lcd.begin(16, 2);
    lcd.clear();

}

bool read_knob()
{
    knob_pos = knob.read();
    if(knob_pos != old_knob_pos)
    {
        if(knob_pos > knob_max)
        {
            knob_pos = knob_max;
            knob.write(knob_max);
        }
        if(knob_pos < 0)
        {
            knob.write(0);
            knob_pos = 0;
        }
        old_knob_pos = knob_pos;
        Serial.println(knob_pos);
        return true;
    }
    return false;
}

void loop()
{

    switch(state)
    {
        case WIFI:
            lcd.print("connect wifi");
            lcd.setCursor(0,1);
            lcd.print(ssid);
            start_wifi();
            state = MPD_CONNECT;
            break;
        
        case MPD_CONNECT:
            lcd.clear();
            lcd.print("connect mpd");
            lcd.setCursor(0,1);
            lcd.print(server);

            if(connect_mpd())
                state = MENU_START;
            break;

        case MENU_START:
            lcd.clear();
            lcd.print("menu");
            knob_max = MENU_LAST_ITEM - 1;
            knob.write(last_menu);
            knob_pos = last_menu;
            state = MENU_UPDATE;
            Serial.println("menu start");
            Serial.println(knob_pos);
            break;
        
        case MENU_WAIT:
            //if knob changed update lcd
            if(read_knob()) 
            {
                state = MENU_UPDATE;
            }
            //button
            if(digitalRead(BUTTON) == LOW && next_state >= 0)
            {
                delay(500); //TODO debounce
                Serial.print("nextstate:"); Serial.println(next_state);
                last_menu = knob_pos;

                //ensure MPD is connected
                if(! mpd_connected())
                    state = MPD_CONNECT;
                else
                    state = next_state;
            }
            break;
    
        case MENU_UPDATE:
            switch(knob_pos)
            {
                case MENU_VOLUME:
                    lcd.setCursor(0,1);
                    lcd.print("volume          ");
                    next_state = VOLUME_START;
                    break;
                case MENU_RANDOM:
                    lcd.setCursor(0,1);
                    lcd.print("play random     ");
                    next_state = RANDOM;
                    break;
                case MENU_FIP:
                    lcd.setCursor(0,1);
                    lcd.print("play FIP        ");
                    next_state = FIP;
                    break;
            }
            state = MENU_WAIT;
            break;

        case VOLUME_START:
            Serial.println("change volume");
            lcd.clear();
            lcd.print("volume");
            knob.write(last_volume);
            knob_pos = last_volume;
            knob_max = 100;
            state = VOLUME_UPDATE;
            break;

        case VOLUME_WAIT:
            if(read_knob())
                state = VOLUME_UPDATE;

            if(digitalRead(BUTTON) == LOW)
            {
                delay(500); //TODO debounce
                last_volume = knob_pos;
                state = MENU_START;
            }
            break;

        case VOLUME_UPDATE:
            lcd.setCursor(0,1);
            lcd.print(knob_pos);
            lcd.print("  ");
            set_vol(knob_pos);
            state = VOLUME_WAIT;
            break;
            
        case RANDOM:
            Serial.println("play random");
            menu_play_random();
            state = MENU_START;
            break;

        case FIP:
            Serial.println("play fip");
            menu_play_fip();
            state = MENU_START;
            break;
    }
}

void menu_play_fip()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("loading fip");
    lcd.setCursor(0,1);
    if(!clear_playlist())
    {
        lcd.print("mpd error");
        delay(500);
        return;
    }

    client.print("load fip\n");
    delay(10);

    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
    {
        lcd.print("mpd error");
        delay(500);
        return;
    }

    lcd.print("loaded..");

    if(!play())
    {
        lcd.print("mpd error");
        delay(500);
        return;
    }
    delay(500);
}

void menu_play_random()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("loading random");
    lcd.setCursor(0,1);
    if(!clear_playlist())
    {
        lcd.print("mpd error");
        delay(500);
        return;
    } 

    if(!load_random_album())
    {
        lcd.print("mpd error");
        delay(500);
        return;
    }

    if(!play())
    {
        lcd.print("mpd error");
        delay(500);
        return;
    }


    lcd.clear();
    lcd.autoscroll();
    lcd.setCursor(16,1);
    String album = get_current_album();
    for(int c = 0; c < album.length(); c ++)
    {
        lcd.print(album.charAt(c));
        delay(150);
    }
    lcd.noAutoscroll();
    lcd.clear(); //have to call clear before printing works again
}

bool mpd_connected()
{
    Serial.println("try to connect");
    client.print("clearerror\n");
    delay(10);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
    return true;
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
    randomSeed(ESP.getCycleCount());
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
            current_album = value + " - ";
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

