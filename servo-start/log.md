## Fri Dec  4 23:26:19 GMT 2015

investigating why keep getting serial timeout on the unit tests
 fails about 5 in 20 of 500 sends :

    while true; do python test_arduino.py TestBuffer.test_keep_buffer_full >> log 2>&1 ; done
* put led in servo code to see how much space is left, plenty
* put led in servo code serial read, find that when it fails the servo doesn't get it's message
* commented out sserial stuff, starst working
* uncomment, fails
* change TX pin from A5 to A4, seems to start working again??!!
