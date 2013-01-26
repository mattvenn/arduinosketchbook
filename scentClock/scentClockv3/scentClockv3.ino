/* 

 - test all pins
 + multisample pots
 + 2 hour release starts one hour before time
 + if a later release interrupts it takes precedence
 + 0 time means off
 - daylight savings calc?
 + heater/fan cycles while heater on
 - el light on for cycle
 - pc connection for monitoring

firmware updates by MV
 + if pot set to 0, will also turn off that heater if it's on
 - let Colin know about RTC errors

*/
//pin defs
const int heaters = 3;
const int pots = 3;
const int heater_pins[heaters] = {6,7,8};
const int pot_pins[pots] = {A0,A1,A2};
const int switch_pin = 10;
const int fan_pin = 11;
const int el_pin = 12;

//structs
struct time
{
    int hour;
    int min;
};

struct heater
{
    boolean heating;
    int heater;
    struct time start_time;
};

//globals
const int samples = 5; //multisample
struct heater heater_on;

//definitions for the heat/fan cycle
const int heat_val = 200;
const int fan_val = 100;
const int cycle_length = 10;
const int heat_length = 3;
const int fan_delay = 3; //fan starts and continues until cycle repeats

//setup
void setup()
{
    Serial.begin(57600);
    Serial.println("started");
    setupRTC();
    updateTime(6,59);
    //pin setup
    for(int i=0; i<heaters; i++)
    {
        pinMode(heater_pins[i],OUTPUT);
        digitalWrite(heater_pins[i],LOW);
    }
    pinMode(fan_pin,OUTPUT);
    digitalWrite(fan_pin,LOW);
    pinMode(switch_pin,INPUT);
    digitalWrite(switch_pin,LOW); //check this

    //globals
    heater_on.heating = false;
}


void loop()
{
    if(Serial.available())
    {
        delay(100);
        int hour = serReadInt();
        int min = serReadInt();
        updateTime(hour,min); 
    }

    struct time now = getTime();
    delay(1000);
    if( now.hour == -1 )
        return;

    //check to see if we need to put a heater on
    for(int pot=0; pot<pots; pot++)
    {
        struct time t = get_time(pot);
        printTime(t);
        Serial.print(",");
        //if pot at 0, then means off, turn off heater too
        if( t.hour == 0 )
        {
            if( heater_on.heating == true && heater_on.heater == pot )
                heater_on.heating = false;
            continue;
        }
        //because this runs in a loop from low to high time, if there is an overlap, the later time will take precence
        if( t.hour - 1 == now.hour && t.min == now.min )
        {
            heater_on.heating = true;
            heater_on.heater = pot;
            heater_on.start_time.min = t.min;
            heater_on.start_time.hour = t.hour - 1;
        }
    }
    Serial.println("");

    //check to see if we need to turn a heater off
    if( heater_on.heating && heater_on.start_time.hour + 2 == now.hour && heater_on.start_time.min == now.min )
    {
        heater_on.heating = false;
    }
    
    if( heater_on.heating )
    {
        //debugging
        digitalWrite(el_pin,HIGH);
        Serial.print("heating:");
        Serial.println(heater_on.heating);
        Serial.print("heater:");
        Serial.println(heater_on.heater);
        Serial.print("started:");
        Serial.print(heater_on.start_time.hour);
        Serial.print(":");
        Serial.println(heater_on.start_time.min);
        
        //heat/fan cycle
        int minutes = (now.hour - heater_on.start_time.hour) * 60;
        minutes += now.min - heater_on.start_time.min;
        Serial.print("minutes into period:");
        Serial.println(minutes);

        int cycle_minute = minutes % cycle_length;
        Serial.print("minutes into cycle:");
        Serial.println(cycle_minute);

        //heat comes on from 0 -> heat_length
        if( cycle_minute < heat_length )
        {
            //analogWrite(heater_pins[heater_on.heater],heat_val);
            Serial.print("heater ");
            Serial.print(heater_on.heater);
            Serial.print(" on at:");
            Serial.println(heat_val);
        }
        else
        {
            Serial.println("heater off");
            //analogWrite(heater_pins[heater_on.heater],0);
        }

        //fan comes on from fan_delay until fan_delay+fan_length
        if( cycle_minute >= fan_delay )
        {
            Serial.print("fan on at:");
            Serial.println(fan_val);
            //analogWrite(fan_pin,fan_val);
        }
        else
        {
            Serial.println("fan off");
            //analogWrite(fan_pin,0);
        }
    }
    else
    {
        digitalWrite(el_pin,LOW);
        Serial.println("heater off");
    }
}
