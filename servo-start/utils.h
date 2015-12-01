#ifndef utils_h
#define utils_h

#include "Arduino.h"

#define STALL_CURRENT 2.0 // amps


byte CRC8(char *data, byte len);
float read_current();
void drive(int yn);

#endif
