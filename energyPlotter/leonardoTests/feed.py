#!/usr/bin/python
import re
import tty
import time
import string

polar = True

gcode = open( "rosie.polar" )
gcodes = gcode.readlines()
serial = None
port = '/dev/ttyACM0'
serial = open( port, 'r+' )
tty.setraw(serial);
#print "wait for port"
#//time.sleep( 1)

def readResponse():
  response = ""
  while string.find(response,"ok"):
    response = serial.readline()
    print response
    if response == "ok\n":
      print "got ok"

p = re.compile( "^#" )

for line in gcodes:
  if p.match(line):
    print "skipping line:", line
  else:
    print line
    serial.write(line)
    readResponse()
