#!/bin/bash
while true; do
  if ./fetchlight.py; then
    echo ---
    rm -f squares.svg 
    ./squares.py --load

    if [ -f squares.svg ] ; then 
     pycam squares.svg --export-gcode=squares.ngc --process-path-strategy=engrave -q
     ./preprocess.py --file squares.ngc  --yoffset 1500 > squares.polar
     ./feed.py --file squares.polar --speed 6
    fi
  fi
  sleep 5
    echo ---
done
