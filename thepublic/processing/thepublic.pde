import processing.serial.*;
import dmxP512.*;

DmxP512 dmxOutput;
int universeSize=128;

Serial myPort;                       // The serial port
int bgColor;

void setup()
{
   size(256, 256,JAVA2D);
   
   String portName = "/dev/ttyUSB1";
    println(Serial.list());
   myPort = new Serial(this, portName, 57600);

   dmxOutput=new DmxP512(this,universeSize,false);
   dmxOutput.setupDmxPro("/dev/ttyUSB0",115000);
  
   
}

void draw() {
  background(bgColor);
  //fill(fgcolor);
  // Draw the shape
//  ellipse(xpos, ypos, 20, 20);
  myPort.write("A");
  dmxOutput.set(1,bgColor);

  //delay(100);

}

void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
//println(binary(inByte));
  if(inByte == 0)
    bgColor = 0;
  else
    bgColor = 255;

}
