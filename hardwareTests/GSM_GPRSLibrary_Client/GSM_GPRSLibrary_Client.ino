/*
author: Matt Venn

description:
based on GSM_GPRSLibrary_Client for GSM Shield for Arduino: www.open-electronics.org

Post data to a web based logging service. To start with using Cosm, moving to a different service later.

probs with original library code:
    even with module off, software serial receives garbage, so init fails
    timeout on software serial doesn't work all the time - seems better after a power cycle
    slow serial reads/writes? check power.

probs with getting a connection
at+creg shows no network
blink light is showing no connection
at+csq shows bad signal.

*/

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#define testMem
InetGSM inet;
int maxGSMTries = 3;
char msg[50];
int numdata;
int i=0;
double lastPost = 0;
String updateString;
int led = 13;

void setup() 
{
  pinMode(led,OUTPUT);
  digitalWrite(led,true);
  //Serial connection.
  Serial.begin(9600);
  Serial.println(F("GSM Shield testing."));
  update();
};

bool startGSM()
{
  //startup at 4800
  if(gsm.begin(4800))
  {
    Serial.println(F("GSM started"));

    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.
    //failing to attach can be a signal strength issue. check at+creg and at+csq
    if(inet.attachGPRS("giffgaff.com", "giffgaff", "password"))
        Serial.println(F("attached to apn"));
    else
    {
        Serial.println(F("failed to attach"));
        return false;
    }
    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");

    //TODO how low can we go?
    delay(5000);

    //Read until serial buffer is empty.
    gsm.WhileSimpleRead();
    return true;
  }
  else
  {
    Serial.println(F("failed to start GSM"));
    return false;
  }
}

//gsm lib doesn't have a finish
//should check that it's on first. see how in gsm.cpp
void endGSM()
{
    Serial.println(F("turning off gsm"));
    digitalWrite(GSM_ON, HIGH);
    delay(3200);
    digitalWrite(GSM_ON, LOW); 
    //could read some data here, but we do it when we start up.

}
void update()
{
    for( int i = 0; i < maxGSMTries; i ++)
    {
        Serial.print(F("trying to start GSM: "));
        //serial.flush() doesn't flush any more, so just read out the chars to avoid the gsm startup failing
        Serial.print(F("reading any serial chars first"));
        while(Serial1.available())
          Serial.write(Serial1.read());
        Serial.println(i);
        if( startGSM() )
        {
            Serial.print(F("making update:"));
       
            updateString = String("value,");
            updateString+=millis();
            Serial.println(updateString);

            char buff[100]; // Or something long enough to hold the longest file name you will ever use.
            updateString.toCharArray(buff, updateString.length()+1);
            Serial.println(buff);

            numdata = inet.httpPOST("api.cosm.com", 80, "/v2/feeds/127682.csv", buff, msg, 150);
            delay(1000);
            //Print the results.
            Serial.println("\nNumber of data received:");
            Serial.println(numdata);  
            Serial.println("\nData received:"); 
            Serial.println(msg); 
            break;
        }
    }
    //wait for result to come back
    delay(5000);
    gsm.WhileSimpleRead();
    
    //turn off module
    endGSM();
}
void loop() 
{
  delay(1000);
  Serial.println( millis());
  
  //every 5 minutes
  if( millis() > lastPost + 300000)
  {
    Serial.println( "update");
    lastPost = millis();
    update();
    Serial.println(freeMemory());
  }

  if( Serial.available() )
  {
    char c = Serial.read();
    switch(c)
    {
        case 'u':
            update();
            break;
        default:
            Serial.println("no such command");
            break;
    }
  }
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
//  serialhwread();
  //Read for new byte on NewSoftSerial.
 // serialswread();
};
/*
void serialhwread(){
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}
*/
#ifdef testMem
extern unsigned int __data_start;
extern unsigned int __data_end;
extern unsigned int __bss_start;
extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

int freeMemory() {
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
};
#endif

