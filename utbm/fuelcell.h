#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

//data struct
typedef struct
{
    int fuelcellStatus;
    float fuelcellAmbientT;
    float fuelcellStackV;
    float fuelcellStackI;
    float fuelcellStackT;
    float fuelcellOutputCurrent;// 
    float fuelcellBatteryV;
  byte id;
  int cksum;
  unsigned long uptime;
} Message;
Message message;

void printFuelCellStatus () 
{
  Serial.print("Status:            "); Serial.print(message.fuelcellStatus, DEC);         Serial.println("");
  Serial.print("Ambient Temp:      "); Serial.print(message.fuelcellAmbientT);       Serial.println(" C");  
  Serial.print("Stack Voltage:     "); Serial.print(message.fuelcellStackV);         Serial.println(" V");  
  Serial.print("Stack Current:     "); Serial.print(message.fuelcellStackI);         Serial.println(" A");  
  Serial.print("Stack Temp:        "); Serial.print(message.fuelcellStackT);         Serial.println(" C");  
  Serial.print("Output Current:    "); Serial.print(message.fuelcellOutputCurrent);  Serial.println(" A");  
  Serial.print("id:                "); Serial.print(message.id, DEC );               Serial.println("");
  Serial.print("uptime:            "); Serial.print(message.uptime );                Serial.println(" ms");
}

void initialiseMessage()
{
  message.fuelcellStatus = 0;
  message.fuelcellAmbientT = 0;
  message.fuelcellStackV = 0;
  message.fuelcellStackI = 0;
  message.fuelcellStackT = 0;
  message.fuelcellOutputCurrent = 0;
  message.id =0;
  message.uptime = 0;
}
