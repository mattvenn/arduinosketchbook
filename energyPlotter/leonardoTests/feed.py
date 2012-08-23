#!/usr/bin/python
import re
import time

polar = True

gcode = open( "rosie.polar" )
gcodes = gcode.readlines()
serial = None
port = '/dev/ttyACM0'
serial = open( port, 'r+' )
print "wait for port"
time.sleep( 1)
import string
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
    print "skip"
  else:
    print line
    serial.write(line)
    readResponse()
