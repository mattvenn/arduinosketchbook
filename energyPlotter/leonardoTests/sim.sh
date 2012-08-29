#!/bin/bash
while true; do
  if ./fetchlight.py; then
    echo ---
    ./squares.py --load

    if [ -f squares.svg ] ; then 
     pycam squares.svg --export-gcode=squares.ngc --process-path-strategy=engrave -q
     ./preprocess.py --file squares.ngc  --yoffset 1500 > squares.polar
     ./feed.py --file squares.polar --speed 6
      timestamp=$(date +%s)
      mv squares.svg history/$timestamp.svg
      ./concat.py
    fi
  fi
  sleep 5
    echo ---
done
