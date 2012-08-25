#!/usr/bin/python
import re
import tty
import time
import string
import argparse
import signal
TIMEOUT = 0.5 # number of seconds your want for timeout

def handler():
  print "timed out on serial read"

signal.signal(signal.SIGALRM, handler)


def readResponse(serial,timeout=10):
  response = ""
  print "setting timeout to", timeout
  while string.find(response,"ok"):
    try:
      if(timeout > 0):
        signal.alarm(timeout)

      response = serial.readline()
      signal.alarm(0)
      print response
      if response == "ok\n":
        print "got ok"
    except:
      print "timeout on read"
      return

def readFile(args):

  try:
    gcode = open( args.file)
  except:
    print "bad file"
    exit(1)
  gcodes = gcode.readlines()
  serial = None
  port = '/dev/ttyACM0'
  serial = open( port, 'r+' )
  tty.setraw(serial);

  p = re.compile( "^#" )

  if args.home:
    serial.write("c")
    readResponse(serial,0)

   #speed and pwm
  serial.write("p%d,%d" % (args.speed, args.pwm ))
  readResponse(serial)

  for line in gcodes:
    if p.match(line):
      print "skipping line:", line
    else:
      print line
      serial.write(line)
      readResponse(serial)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="feed polar files to polargraph robot")
    parser.add_argument('--file',
        action='store', dest='file', 
        help="file to open")
    parser.add_argument('--home',
        action='store_const', const=True, dest='home', default=False,
        help="home to start")
    parser.add_argument('--pwm',
        action='store', dest='pwm', type=int, default=80,
        help="pwm to draw")
    parser.add_argument('--speed',
        action='store', dest='speed', type=int, default=3,
        help="speed to draw")

    args = parser.parse_args()

    #set values, must be a better way of doing this
    readFile(args)
