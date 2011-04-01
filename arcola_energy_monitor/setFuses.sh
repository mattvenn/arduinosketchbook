#!/bin/bash
avrdude -B 100 -V -p m328p -P /dev/ttyACM0 -c stk500v2 -U lfuse:w:0xff:m
avrdude -B 1 -V -p m328p -P /dev/ttyACM0 -c stk500v2 -U hfuse:w:0xd1:m
avrdude -B 1 -V -p m328p -P /dev/ttyACM0 -c stk500v2 -v
echo fuses should be
echo ff
echo d1
