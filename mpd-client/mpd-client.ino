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

#include <ESP8266WiFi.h>
#include "secrets.h"
#include <Encoder.h>
#include <LiquidCrystal.h>

enum states
{
    WIFI_START,
    WIFI_WAIT,
    MPD_CONNECT,
    MENU_START,
    MENU_WAIT,
    MENU_UPDATE,
    VOLUME_START,
    VOLUME_WAIT,
    VOLUME_UPDATE,
    PLAY_FIP,
    PLAY_CADENA,
    PLAY_KLARA,
    PLAY_RANDOM,
    POWER_OFF,
    CONN_FAIL,
};

enum menus
{
    MENU_RANDOM,
    MENU_FIP,
    MENU_KLARA,
    MENU_CADENA,
    MENU_VOLUME,
    MENU_LAST_ITEM, // MUST BE THE LAST ITEM //
};

const int knob_multi = 8; // knob is too sensitive
int knob_pos = 0;
int old_knob_pos = 0;
int knob_max = 0;
int last_volume = 0;
int last_menu = 0;
int connect_attempts = 0;
unsigned long power_timer = 0;
int state = WIFI_START;
int next_state = VOLUME_START; //used for transferring state on button press, should match the state required for the 1st menu item

#define SCROLL_SPEED 100 // ms scroll speed for showing current playing
#define POWER_TIME 15000 // ms after before power off
#define MPD_WAIT 100 // ms to wait for mpd replies
#define DISPLAY_TIMEOUT 1000 // ms for display to time out
#define CONN_ATTEMPTS 10 // how many times to attempt to connect before power off
#define CONN_TIMEOUT 1000 // ms wait in between each conn attempt
#define DEBOUNCE 200 //crap button debouncing

#define BUTTON A0
#define POWER 13

Encoder knob(4,5); //didn't work with 13, 15, 14, 16
LiquidCrystal lcd(0, 2, 15, 12, 14, 16);
IPAddress server(192, 168, 1, 241);
WiFiClient client;

void setup()
{
    pinMode(POWER, OUTPUT); //has external pullup
    digitalWrite(POWER, HIGH);

    Serial.begin(9600);
    Serial.println();
    Serial.println();

    lcd.begin(16, 2);
    lcd.clear();
}

void loop()
{
    //shutdown after timeout
    if(millis() - power_timer > POWER_TIME)
        state = POWER_OFF;

    //state machine
    switch(state)
    {
        case WIFI_START:
            WiFi.begin(ssid, password);
            state = WIFI_WAIT;
            break;

        case WIFI_WAIT:
            power_timer = millis(); // prevent early shutdown
            lcd.clear();
            lcd.print("connect wifi: ");
            lcd.print(CONN_ATTEMPTS - connect_attempts);
            lcd.setCursor(0,1);
            lcd.print(ssid);

            if(WiFi.status() == WL_CONNECTED) 
            {
                Serial.println("WiFi connected");  
                Serial.println("IP address: ");
                Serial.println(WiFi.localIP());
                state = MPD_CONNECT;
                connect_attempts = 0;
            }
            else
            {
                connect_attempts ++;
                delay(CONN_TIMEOUT);
            }

            if(connect_attempts > CONN_ATTEMPTS)
                state = CONN_FAIL;

            break;
        
        case MPD_CONNECT:
            power_timer = millis(); // prevent early shutdown
            lcd.clear();
            lcd.print("connect mpd: ");
            lcd.print(CONN_ATTEMPTS - connect_attempts);
            lcd.setCursor(0,1);
            lcd.print(server);

            if(connect_mpd())
            {
                state = MENU_START;
                connect_attempts = 0;
            }
            else
            {
                connect_attempts ++;
                delay(CONN_TIMEOUT);
            }

            if(connect_attempts > CONN_ATTEMPTS)
                state = CONN_FAIL;

            break;

        case MENU_START:
            lcd.clear();
            lcd.print("menu");
            knob_max = MENU_LAST_ITEM - 1;
            knob.write(last_menu*knob_multi);
            knob_pos = last_menu;
            state = MENU_UPDATE;
            Serial.println("menu start");
            break;
        
        case MENU_WAIT:
            //if knob changed update lcd
            if(read_knob()) 
            {
                state = MENU_UPDATE;
            }
            //button
            if(button_pressed())
            {
                last_menu = knob_pos;

                //ensure MPD is connected
                if(! mpd_connected())
                    state = MPD_CONNECT;
                else
                    state = next_state;
            }

            break;
    
        case MENU_UPDATE:
            lcd.setCursor(0,1);
            switch(knob_pos)
            {
                case MENU_RANDOM:
                    lcd.print("play random     ");
                    next_state = PLAY_RANDOM;
                    break;
                case MENU_FIP:
                    lcd.print("play FIP        ");
                    next_state = PLAY_FIP;
                    break;
                case MENU_KLARA:
                    lcd.print("play Klara      ");
                    next_state = PLAY_KLARA;
                    break;
                case MENU_CADENA:
                    lcd.print("play Cadena Ser ");
                    next_state = PLAY_CADENA;
                    break;
                case MENU_VOLUME:
                    lcd.print("volume          ");
                    next_state = VOLUME_START;
                    break;
            }
            state = MENU_WAIT;
            break;

        case VOLUME_START:
            Serial.println("change volume");
            lcd.clear();
            last_volume = get_mpd_volume();
            lcd.print("set volume %");
            knob.write(last_volume*knob_multi);
            knob_pos = last_volume;
            knob_max = 100;
            state = VOLUME_UPDATE;
            break;

        case VOLUME_WAIT:
            if(read_knob())
                state = VOLUME_UPDATE;

            if(button_pressed())
                state = MENU_START;
            break;

        case VOLUME_UPDATE:
            lcd.setCursor(0,1);
            lcd.print(knob_pos);
            lcd.print("  ");
            set_vol(knob_pos);
            state = VOLUME_WAIT;
            break;
            
        case PLAY_RANDOM:
            Serial.println("play random");
            menu_play_random();
            state = MENU_START;
            break;

        case PLAY_CADENA:
            Serial.println("play cadena");
            menu_playlist("cadena ser");
            state = MENU_START;
            break;

        case PLAY_KLARA:
            Serial.println("play KLARA");
            menu_playlist("klara");
            state = MENU_START;
            break;

        case PLAY_FIP:
            Serial.println("play FIP");
            menu_playlist("fip");
            state = MENU_START;
            break;

        case CONN_FAIL:
            lcd.clear();
            lcd.print("connection");
            lcd.setCursor(0,1);
            lcd.print("failed");
            delay(DISPLAY_TIMEOUT);
            state = POWER_OFF;
            break;

        case POWER_OFF:
            lcd.clear();
            lcd.print("power off");
            delay(DISPLAY_TIMEOUT);
            digitalWrite(POWER, LOW);
            break;
    }
}

bool button_pressed()
{
    if(analogRead(BUTTON) > 750)
    {
        power_timer = millis(); //something happened
        delay(DEBOUNCE);
        return true;
    }
    return false;
}

bool read_knob()
{
    knob_pos = knob.read(); 
    if(knob_pos != old_knob_pos)
    {
        power_timer = millis(); //something happened
//        Serial.print("A0="); Serial.println(analogRead(A0));
        if(knob_pos > knob_max * knob_multi)
        {
            knob_pos = knob_max * knob_multi;
            knob.write(knob_max * knob_multi);
        }
        if(knob_pos < 0)
        {
            knob.write(0);
            knob_pos = 0;
        }
        old_knob_pos = knob_pos;
        knob_pos /= knob_multi;
        return true;
    }
    knob_pos /= knob_multi;
    return false;
}

void menu_playlist(String name)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("loading "); 
    lcd.print(name);
    lcd.setCursor(0,1);
    if(!clear_playlist())
    {
        lcd.print("mpd error");
        delay(DISPLAY_TIMEOUT);
        return;
    }

    client.print("load ");
    client.print('"');
    client.print(name);
    client.print('"');
    client.print('\n');
    delay(MPD_WAIT);

    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
    {
        lcd.print("mpd error");
        delay(DISPLAY_TIMEOUT);
        return;
    }

    lcd.print("loaded");

    if(!play())
    {
        lcd.print("mpd error");
        delay(DISPLAY_TIMEOUT);
        return;
    }
    delay(DISPLAY_TIMEOUT);
}

void menu_play_random()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("loading random");
    lcd.setCursor(0,1);
    if(!clear_playlist())
    {
        lcd.print("err: playlist");
        delay(DISPLAY_TIMEOUT);
        return;
    } 

    if(!load_random_album())
    {
        lcd.print("err: load rand");
        delay(DISPLAY_TIMEOUT);
        return;
    }

    if(!play())
    {
        lcd.print("err: play");
        delay(DISPLAY_TIMEOUT);
        return;
    }


    lcd.clear();
    lcd.autoscroll();
    lcd.setCursor(16,1);
    String album = get_current_album();
    for(int c = 0; c < album.length(); c ++)
    {
        lcd.print(album.charAt(c));
        delay(SCROLL_SPEED);
    }
    lcd.noAutoscroll();
    lcd.clear(); //have to call clear before printing works again
}

// lower level MPD related stuff //

bool mpd_connected()
{
    Serial.println("try to connect");
    client.print("clearerror\n");
    delay(MPD_WAIT);
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
    delay(MPD_WAIT);
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
    delay(MPD_WAIT);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
    return true;
}

bool play()
{
    client.print("play\n");
    delay(MPD_WAIT);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
}

bool set_vol(int vol)
{
    client.print("setvol ");
    client.print(vol);
    client.println();
    delay(MPD_WAIT);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
}

int get_mpd_volume()
{
    Serial.println("getting mpd volume");
    int current_vol = 0;
    //request current song
    client.print("status\n");
    delay(MPD_WAIT);
    
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
        if(field == "volume")
            current_vol = value.toInt();
    }
    return current_vol;
}

bool load_random_album()
{
    randomSeed(ESP.getCycleCount());
    client.print("list album\n");
    delay(MPD_WAIT);

    int albums = 1;
    String choice;
    if(client.available() == 0)
    {
        Serial.println("no reply from MPD");
        return false;
    }    
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
    delay(MPD_WAIT);
    String ack = client.readStringUntil('\n');
    if(! ack.startsWith("OK"))
        return false;
    return true;
} 

String get_current_album()
{
    String current_album = "no album playing";
    //request current song
    client.print("currentsong\n");
    delay(MPD_WAIT);
    
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
        if(field == "Artist")
            current_album = value + " - ";
        if(field == "Album")
            current_album += value;
    }
    return current_album;
}
