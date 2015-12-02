#ifndef pindefs_h
#define pindefs_h

#define ENCB 3               //hardware ints
#define ENCA 2               //hardware ints
#define SerialTxControl 4    //RS485 Direction control
#define SLAVE_TX 5           //software serial
#define SLAVE_RX 6           //software serial
#define SSerialTxControl 7   //RS485 Direction control
#define REV 9                //timer 1
#define FOR 10               //timer 1
#define LED 13
#define LED_ERROR 12
#define CURRENT A5 //current of DC motor, using ACS712 185mV/A, 2.5v = 0A

#endif
