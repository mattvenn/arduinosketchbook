#!/usr/bin/python
import json
import os
import pickle
import iso8601

#fetch data
os.system("curl --request GET http://api.cosm.com/v2/feeds/46756 --header 'X-ApiKey: QHcIMwn4vsbSC3kgzClHrh_3XdiSAKw0b1dvY1VBV3JQRT0g' -s > json" )
datafh = open( "json" )
data = json.load(datafh)
light = data["datastreams"][0]["current_value"]
time = data["updated"]

#only progress if there has been a change
laststate = pickle.load( open( "vars.p", "rb" ) )
if time == laststate["time"]:
  print "no change"
  exit(1)

date = iso8601.parse_date(time)
minute = int( date.strftime("%M") ) # minute 0 -59
hour = int(date.strftime("%H") ) # hour 0 -23
mins =  (minute + hour * 60)/10 # 0 - 143

print "time: %s\nmins: %d\nlight %s" % ( time, mins, light)
#store vals
loadedvars = { "number" : mins, "env" : float(light), "time" : time } 
pickle.dump( loadedvars, open( "vars.p", "wb" ) )

