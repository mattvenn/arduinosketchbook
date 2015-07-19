import requests
from datetime import datetime

config = [
    { 'name' : 'temp', 'public' : 'q5X5m25pp8FYYgZ0EQld' },
    { 'name' : 'pi', 'public' : 'MGw10nX37ZSbA2A463Xo' },
]

base_url = 'https://data.sparkfun.com/output/'
from_time = '2015-07-19T12:00' 
from_time_dt = datetime.strptime(from_time, '%Y-%m-%dT%H:%M')
dt = datetime.now() - from_time_dt
exp_rows = float(dt.seconds / 60)  # one post per minute

for c in config:
    payload = { 'gte[timestamp]': from_time }
    r = requests.get(base_url + c['public'], params=payload)
    if r.status_code == 200:
        rows = len(r.text.splitlines())
        print("%5s: %d / %d = %02d%%" %
            (c['name'], rows, exp_rows, (rows/exp_rows)*100))
