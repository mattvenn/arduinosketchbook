#!/bin/bash
port=/dev/ttyUSB0

#line passing
#commands="ATRE,ID3456,MY2,DL1,D25,P02,P12,IU1,IA1,WR" #for output module

#commands="ATRE,ID3456,MY1,DL2,D13,IR14,IT1,WR" #for input module, sends to output
# commands="ATRE,ID3456,MY1,DL2,D02,D12,D23,IRC8,IT1,SM5,SP28,ST1,WR" # low power (sleeping) input module. gas pulse on d2(p18), current on d1(19) and battv on d0(20)

#simple
#commands="ATRE,ID3456,MY1,DL2,SM1,WR" #for remote 
commands="ATRE,ID3456,MY2,DL3,IU1,WR" #for nanode
#commands="ATRE,ID3456,MY3,DL2,WR" #for robot

# RE reload defaults
# MY my address
# DL which ID to send to
# ID pan address
# D0 - IO line 0  (0 disabled,  2 adc,  3 di, 4  do low, 5 do high
# D1 - IO line 1
# IR sample rate x ms
# IT samples to buffer
# IU enable/disable sending data out the uart
# IA (to do with line passing)

# SM - sleep mode 4 = cyclic, 5 cyclic with wake
# ST - time before slep  x 1ms
# SP - cyclic sleep period x 10ms


echo -n +++  > $port
sleep 4
echo -ne $commands\\r > $port

#for i in $commands ; do
#    echo $i
#    echo -ne AT$i\\r > $port
#    sleep 1
#done

#> /dev/ttyUSB1 ; sleep 2; echo -ne ATIR14\\r > /dev/ttyUSB1 

#ATID3456 –> PAN ID
#ATMY2 –> my address 2
#ATDL1 –> destination address 1
#ATP02 –> PWM 0 in PWM mode
#ATD15 –> output 1 in digital out high mode
#ATIU1 –> I/O output enabled
#ATIA1 –> I/O input from address 1
#ATWR –> write settings to firmware
