#!/usr/bin/python

gcode = open( "rosie.ngc" )
xoffset = 1000
ysub = 1000
yoffset = 500
scale = 30
import re

#start the file
print "p3,100"
print "c"

gcodes = gcode.readlines()
startCode = re.compile( "^G([01])(?: X(\S+))?(?: Y(\S+))?(?: Z(\S+))?$")
contCode =  re.compile( "^(?: X(\S+))?(?: Y(\S+))?(?: Z(\S+))?$")
#p = re.compile( "G([01])(?= Z(\S+))")
for line in gcodes:
  s = startCode.match(line)
  c = contCode.match(line)
  gcode = 0
  if s:
    gcode = s.group(1)
    x = s.group(2)
    y = s.group(3)
    z = float(s.group(4))
    if z > 0 :
      print "s600,2"
    else:
      print "s200,2"
  elif c: 
    try:
      x = float(c.group(1))
    except:
      x = lastX 
    try:
      y = float(c.group(2))
    except:
      y = lastY
#    z = float(c.group(3))
#    print line
    print "g%d,%d" %  (x*scale+xoffset,ysub - y*scale+yoffset) 
    lastX = x
    lastY = y

