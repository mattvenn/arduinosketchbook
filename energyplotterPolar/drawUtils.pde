
float rads(int n) {
  // Return an angle in radians
  return (n/180.0 * PI);
}

void moveTo(int x2, int y2) {
  
  // Turn the stepper motors to move the marker from the current point (x1,
  // y1) to (x2, y2)
  // Note: This only moves in a perfectly straight line if 
  // the distance is the same in both dimensions; this should be fixed, but it
  // works well
  
  // a2 and b2 are the final lengths of the left and right strings
  int a2 = sqrt(pow(x2,2)+pow(y2,2));
  int b2 = sqrt(pow((w-x2),2)+pow(y2,2));
  int stepA;
  int stepB;
  if (a2>a1) { 
    stepA=1; 
  }
  if (a1>a2) { 
    stepA=-1;
  }
  if (a2==a1) {
    stepA=0; 
  }
  if (b2>b1) { 
    stepB=1; 
  }
  if (b1>b2) { 
    stepB=-1;
  }
  if (b2==b1) {
    stepB=0; 
  }

  // Change the length of a1 and b1 until they are equal to the desired length
  while ((a1!=a2) || (b1!=b2)) {
    if (a1!=a2) { 
      a1 += stepA;
      leftStepper.step(stepA); 
    }
    if (b1!=b2) { 
      b1 += stepB;
      rightStepper.step(stepB);
    }
  }
  x1 = x2;
  y1=y2;
}

void drawCurve(float x, float y, float fx, float fy, float cx, float cy) {
  // Draw a Quadratic Bezier curve from (x, y) to (fx, fy) using control pt
  // (cx, cy)
  float xt=0;
  float yt=0;

  for (float t=0; t<=1; t+=.0025) {
    xt = pow((1-t),2) *x + 2*t*(1-t)*cx+ pow(t,2)*fx;
    yt = pow((1-t),2) *y + 2*t*(1-t)*cy+ pow(t,2)*fy;
    moveTo(xt, yt);
  }  
}
                                                     
void drawCircleSegment(int centerx, int centery, int radius, int startSeg, int endSeg) {
  
    // Estimate a circle using 20 arc Bezier curve segments
//  println( centerx + "," + centery + ", " + radius + ", " + startSeg + ", " + endSeg );
  int segments =20;
  int angle1 = (360/segments)*startSeg;
  int midpoint=0;
  
  //little bit of limit handling
  if( endSeg > 20 )
   endSeg = 20;
   
  for (float angle2=(360/segments)*(startSeg+1); angle2<=360/segments*endSeg; angle2+=360/segments) {

    midpoint = (angle1+(angle2-angle1)/2);

    float startx=centerx+radius*cos(rads(angle1));
    float starty=centery+radius*sin(rads(angle1));
    float endx=centerx+radius*cos(rads(angle2));
    float endy=centery+radius*sin(rads(angle2));
    
    int t1 = rads(angle1)*1000 ;
    int t2 = rads(angle2)*1000;
    int t3 = angle1;
    int t4 = angle2;

    drawCurve(startx,starty,endx,endy,
              centerx+2*(radius*cos(rads(midpoint))-.25*(radius*cos(rads(angle1)))-.25*(radius*cos(rads(angle2)))),
              centery+2*(radius*sin(rads(midpoint))-.25*(radius*sin(rads(angle1)))-.25*(radius*sin(rads(angle2))))
    );
    
    angle1=angle2;
  }  
}

void drawCircle(int centerx, int centery, int radius) {
  moveTo(centerx+radius, centery);
  drawCircleSegment(centerx,centery,radius,0,20);
}


              
void drawCircles(int number, int centerx, int centery, int r) {
   // Draw a certain number of concentric circles at the given center with
   // radius r
   int dr=0;
   if (number > 0) {
     dr = r/number;
     for (int k=0; k<number; k++) {
       drawCircle(centerx, centery, r);
       r=r-dr;
     }
   }
}

