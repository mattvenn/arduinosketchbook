
#include <PID_v1.h>
#include <TimedAction.h>
#include <LiquidCrystal.h>


//system variables
#define LOW_BATTERY_VOLTAGE 550
#define DEBUG //define this for PID debugging
#define CURRENT_CHARGING_VALUE 50

//ERRORS
#define ERR_LOW_BATTERY 1

//global vars
//PID vars
double Setpoint, rawBattVoltage, pwmDumpLoad;
int rawCurrent, rawTurbineVoltage;
boolean statusLED = false;
#define numErrors 1
boolean errors[numErrors];
byte history [16];

//timed actions
TimedAction actionUpdateLCD = TimedAction(1000,updateLCD);
TimedAction actionUpdateElectrical = TimedAction(50,updateElectrical);
TimedAction actionCheckBattery = TimedAction(2000,checkBattery); //2 secs
TimedAction actionAddToHistory = TimedAction(300000,addToHistory); //5 minutes
#ifdef DEBUG
TimedAction actionRXTXPID = TimedAction(200,RXTXPID);
#endif

//PID: Specify the links and initial tuning parameters
//for details on tuning params see http://en.wikipedia.org/wiki/PID_controller
PID myPID(&rawBattVoltage, &pwmDumpLoad, &Setpoint,2,5,1, DIRECT); //DIRECT
 
//TODO lcd setup
LiquidCrystal lcd(2, 4, 5, 6, 7, 8);

//control pin defs
#define PIN_pwmDumpLoad 3
//TODO
#define PIN_ledGreen 0
#define PIN_ledRed 0
#define PIN_phoneChargerSwitch 0
//analog input pin defs
#define PIN_battVoltage 0
#define PIN_turbineVoltage 1
#define PIN_current 2


void setup()
{
  Serial.begin(9600);
  setupErrors();
  createBars();
  //pin setups
  pinMode( PIN_ledGreen, OUTPUT );
  pinMode( PIN_ledRed, OUTPUT );
  pinMode( PIN_phoneChargerSwitch, OUTPUT );
  digitalWrite( PIN_ledGreen, LOW );
  digitalWrite( PIN_ledRed, LOW );  
  lcd.begin(16, 2);
  lcd.blink();  
  //initialize the variables we're linked to
  rawBattVoltage = analogRead(PIN_battVoltage);
  Setpoint = 650;
  myPID.SetSampleTime(200); //ms
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop()
{

  myPID.Compute();
  analogWrite(PIN_pwmDumpLoad,pwmDumpLoad);
  
  //repeating stuff
  actionUpdateLCD.check();
  actionUpdateElectrical.check();
  actionCheckBattery.check();
  actionAddToHistory.check();
  
#ifdef DEBUG 
  actionRXTXPID.check();
#endif
  
}

void updateElectrical()
{
  rawBattVoltage = analogRead(PIN_battVoltage);
  rawCurrent = analogRead(PIN_current);
  rawTurbineVoltage = analogRead(PIN_turbineVoltage); 
}

void checkBattery()
{
  if( rawBattVoltage < LOW_BATTERY_VOLTAGE )
  {
    digitalWrite( PIN_phoneChargerSwitch, LOW );
    errors[ERR_LOW_BATTERY] = 1;
  }
  else
  {
    digitalWrite( PIN_phoneChargerSwitch, HIGH );
    errors[ERR_LOW_BATTERY] = 0;
  }
}
