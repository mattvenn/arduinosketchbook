/*

 Toaster Oven SMT soldering control
 Adapted from work by Adrian Bowyer : http://reprap.org/wiki/Toaster_Oven_Reflow_Technique#Firmware
 Matt Venn
 23 Jan 2015
 Licence: GPL
 
 */
#define go A2
#define SSR A3
#define LED 13

//temp sense is from http://www.seeedstudio.com/wiki/Grove_-_High_Temperature_Sensor
#include "High_Temp.h"
#include <LiquidCrystal.h>

HighTemp ht(A0, A1);

// RS, EN, 4,5,6,7
LiquidCrystal lcd(7,6,11,10,9,8);

int heatState = LOW;         // heatState used to set the LED and heater
long previousMillis = 0;     // will store last time LED/heater was updated
const long interval = 1000;  // interval at which to sample temperature (milliseconds)

long time = 0;               // Time since start in seconds
bool done=false;             // Flag to indicate that the process has finished
bool started=false;
// The temperature/time profile as {secs, temp}
// This profile is linearly interpolated to get the required temperature at any time.
// PLEN is the number of entries
#define PLEN 6
long profile[PLEN][2] = { {0, 15}, {120, 150}, {220, 183}, {280, 215}, {320, 183}, {350, 0} };


void setup()
{
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  Serial.begin(115200);
  Serial.println("reflow");
  ht.begin();

  pinMode(go,OUTPUT);
  pinMode(SSR ,OUTPUT);
  pinMode(LED,OUTPUT);

  digitalWrite(LED,LOW);
  digitalWrite(go,HIGH);

  done = false;
}

// Linearly interpolate the profile for the current time in secs, t
int target(long t)
{
  if(t <= profile[0][0])
    return profile[0][1];
  if(t >= profile[PLEN-1][0])
  {
    done = true; // We are off the end of the time curve
    return profile[PLEN-1][1];
  }
  for(int i = 1; i < PLEN-1; i++)
  {
    if(t <= profile[i][0])
      return (int)(profile[i-1][1] + ((t - profile[i-1][0])*(profile[i][1] - profile[i-1][1]))/
        (profile[i][0] - profile[i-1][0]));
  }
  return 0;
}


// Go round and round
void loop()
{
  int t;
  unsigned long currentMillis = millis();
  if(digitalRead(go)==LOW)
    started = true;
  if(started && currentMillis - previousMillis > interval) 
  {
    previousMillis = currentMillis; // set next time 

    // Get the actual temperature
    t = ht.getThmc();

    // One second has passed
    time++;   

    // Find the target temperature
    int tg = target(time);

    // Simple bang-bang temperature control
    if (t < tg)
    {
      heatState = HIGH;
    } 
    else
    {
      heatState = LOW;
    }

    // Turn the heater on or off (and the LED)
    digitalWrite(SSR, heatState);
    digitalWrite(LED, heatState);

    // Keep the user amused
    if(done)
    {
      Serial.print((char)0x07);  // Bell to wake the user up...
      Serial.print((char)0x07);
      Serial.print("FINISHED ");
      started = false;
      done = false;
      time = 0;
    }
    Serial.print(time);
    Serial.print(", ");
    Serial.print(tg);
    Serial.print(", ");
    Serial.println(t);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("t=");
    lcd.print(time);
    lcd.setCursor(8, 0);
    lcd.print("oven=");
    lcd.print(heatState ? "on" : "off");
    // print the number of seconds since reset:
    lcd.setCursor(0,1);
    lcd.print("tgt=");
    lcd.print(tg);
    lcd.setCursor(8,1);
    lcd.print("t=");
    lcd.print(t);
  }

  if(started == false)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("reflow oven");
    lcd.setCursor(0,1);
    lcd.print("press to start");
    delay(50);
  }
}

