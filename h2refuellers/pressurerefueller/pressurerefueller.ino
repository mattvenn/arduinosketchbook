#include <EEPROM.h>

//leds
static const int led_1 = 8;
static const int led_2 = 9;
static const int led_3 = 10;
static const int led_4 = 11;
static const int led_5 = 12;
static const int led_6 = 13;
static const int numLeds = 6;
static const int ledPinStart = 8;
//io
static const int PURGE = 3;
static const int PRESSURE = A5;
static const int SWITCH = 7;

//global vars
static const int lowHPressure = 100; //low and high hydrostik pressure
static const int maxHPressure = 1000;

static const int lowFullPressure = 500; //low and high syringe fill pressure
static const int pressureIncrement = 50; //what we increase pressure by for each button press
static const int highFullPressure = numLeds * pressureIncrement + lowFullPressure;

static const int timeout = 20 * 10; //100s milliseconds
static const float FILTER = 0.9;

int fullPressure; //pressure we fill to, set by eeprom read
int avgPressure;
double checkPressureTimer;

void setup()
{
  Serial.begin(9600);
  Serial.println("started");
  pinMode(PURGE,OUTPUT);
  pinMode(SWITCH,INPUT);
  digitalWrite(SWITCH,HIGH);

  fullPressure = EEPROM.read(0) * 10; 
  Serial.print("fill pressure set to: ");
  Serial.println(fullPressure);
  
  //leds pins
  for( int led = ledPinStart; led < ledPinStart + numLeds; led++)
  {
    pinMode(led,OUTPUT);
  }

  //start avgPressure of as something useful
  avgPressure = getPressure();
  
  //show pressure in the hydrostiks
  showPressure(getPressure(),lowHPressure,maxHPressure);
  
  //allow user to set full pressure
  if(digitalRead(SWITCH)==LOW)
    setFullPressure();
  
}

void loop()
{
  digitalWrite(led_6,HIGH); //turn on green light to show we're ready
  
  if(digitalRead(SWITCH) == LOW)
    charge();

  //work out average pressure
  if(millis() > checkPressureTimer + 1000)
  {
    avgPressure = avgPressure * FILTER  + ( 1 - FILTER ) * getPressure();
    checkPressureTimer = millis();
    Serial.println(avgPressure);
  }  
}


//turn on leds to show pressure
void showPressure(int pressure,int low, int high)
{
  int showLeds = map(pressure,low,high, 1, numLeds);

  for( int led = ledPinStart; led < ledPinStart + showLeds; led++)
  {
    digitalWrite(led,HIGH);
    delay(100);
  }
  delay(500);
  allLedsOff();
}  

void showSetPressure(int pressure)
{
  int showLeds = (pressure - lowFullPressure) / 50;
  for( int led = ledPinStart; led < ledPinStart + showLeds; led++)
  {
    digitalWrite(led,HIGH);
  }
}

void setFullPressure()
{
  showWarning();
  showSetPressure(fullPressure);

  int count = millis();
  boolean debounce = true;
  //one second to make an adjustment
  while(millis() < count + 5000)
  {
    //debounce
    if(digitalRead(SWITCH)==HIGH)
    {
       debounce = true;
       delay(10);
    }
    
    if(digitalRead(SWITCH)==LOW && debounce)
    {
      Serial.println(millis());
      fullPressure += 50;
      count = millis();

      if(fullPressure>highFullPressure)  
      {
        fullPressure=lowFullPressure;
        allLedsOff();
      }
      showSetPressure(fullPressure); 
      debounce = false;
    }
    
  }
  Serial.print("writing to eeprom: ");
  Serial.println(fullPressure);
  EEPROM.write(0,fullPressure/10); //set address
  showWarning();
}
  
void allLedsOff()
{
  //turn off
  for( int led = ledPinStart; led < ledPinStart + numLeds; led++)
  {
    digitalWrite(led,LOW);
  } 
}
void showWarning()
{
  allLedsOff();
  for( int i = 0; i < 5; i ++)
  {
    for( int led = ledPinStart; led < ledPinStart + 3; led++)
    {
      digitalWrite(led,HIGH);
    }
    delay(100);
    allLedsOff();
    delay(100);
  }
}

   
void printPressure()
{
  Serial.println(getPressure());  
}
int getPressure()
{
  return analogRead(PRESSURE);
}
  
void charge()
{
  Serial.println("charge");
  allLedsOff();
  
  if( getPressure() < lowHPressure )
  {
    Serial.println( "low pressure" );
    showWarning();    
    return;
  }

  int count = 0;
  digitalWrite(PURGE,HIGH);
  delay(500); //wait for pressure to equalise

  int startPressure = getPressure();

  while(getPressure() < fullPressure)
  {
   
    int showLeds = map(getPressure(),startPressure,fullPressure, 1, numLeds+1); //num leds + 1 so that all get lit by the end of a good fill
    for( int led = ledPinStart; led < ledPinStart + showLeds; led++)
    {
      digitalWrite(led,HIGH);
    }
  
    delay(100);
    Serial.print(count);
    Serial.print(",");
    printPressure();
       
    if(getPressure() < lowHPressure)
    {
      Serial.println("low pressure - syringe disconnected?");
      digitalWrite(PURGE,LOW);
      showWarning();
      break;
    }    
    if(count > timeout)
    {
      Serial.println("timed out");
      digitalWrite(PURGE,LOW);
      showWarning();
      break;
    }
  }
  digitalWrite(PURGE,LOW);
  delay(500);
  allLedsOff();  
  Serial.println("finished");
}
 
