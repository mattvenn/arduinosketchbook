
float rads(int n) {
  // Return an angle in radians
  return (n/180.0 * PI);
}                                                                        
#ifdef XBEE
void drawCircleGrid()
{
  int circleWidth = (h - ceiling )/ 16;
 for( int x = 0; x < 5; x ++ )

 {
   for( int y = 0; y < 5 ; y ++ )
   {
      drawCircle( x * h / 6 + ( w / 2 - h / 3 ), y * h / 6 + circleWidth + ceiling , circleWidth );
   }
 } 

}
#endif
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
                                                     

void drawCircle(int centerx, int centery, int radius) {
  // Estimate a circle using 20 arc Bezier curve segments
  int segments =20;
  int angle1 = 0;
  int midpoint=0;
   
   moveTo(centerx+radius, centery);

  for (float angle2=360/segments; angle2<=360; angle2+=360/segments) {

    midpoint = angle1+(angle2-angle1)/2;

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

void drawEnergy()
{
  drawCircle( 1000, 643, 3 );
drawCircle( 1000, 643, 13 );
drawCircle( 1000, 643, 23 );
drawCircle( 1000, 643, 33 );
drawCircle( 1000, 643, 43 );
drawCircle( 1000, 643, 53 );
drawCircle( 1133, 643, 3 );
drawCircle( 1133, 643, 13 );
drawCircle( 1266, 643, 3 );
drawCircle( 1266, 643, 13 );
drawCircle( 1399, 643, 3 );
drawCircle( 1399, 643, 13 );
drawCircle( 1532, 643, 3 );
drawCircle( 1665, 643, 3 );
drawCircle( 1665, 643, 13 );
drawCircle( 1798, 643, 3 );
drawCircle( 1931, 643, 3 );
drawCircle( 2064, 643, 3 );
drawCircle( 2197, 643, 3 );
drawCircle( 2197, 643, 13 );
drawCircle( 2330, 643, 3 );
drawCircle( 2463, 643, 3 );
drawCircle( 1000, 776, 3 );
drawCircle( 1000, 776, 13 );
drawCircle( 1000, 776, 23 );
drawCircle( 1000, 776, 33 );
drawCircle( 1000, 776, 43 );
drawCircle( 1133, 776, 3 );
drawCircle( 1133, 776, 13 );
drawCircle( 1266, 776, 3 );
drawCircle( 1399, 776, 3 );
drawCircle( 1399, 776, 13 );
drawCircle( 1532, 776, 3 );
drawCircle( 1665, 776, 3 );
drawCircle( 1798, 776, 3 );
drawCircle( 1931, 776, 3 );
drawCircle( 1931, 776, 13 );
drawCircle( 2064, 776, 3 );
drawCircle( 2064, 776, 13 );
drawCircle( 2064, 776, 23 );
drawCircle( 2197, 776, 3 );
drawCircle( 2197, 776, 13 );
drawCircle( 2197, 776, 23 );
drawCircle( 2197, 776, 33 );
drawCircle( 2197, 776, 43 );
drawCircle( 2197, 776, 53 );
drawCircle( 2197, 776, 63 );
drawCircle( 2197, 776, 73 );
drawCircle( 2197, 776, 83 );
drawCircle( 2197, 776, 93 );
drawCircle( 2197, 776, 103 );
drawCircle( 2197, 776, 113 );
drawCircle( 2197, 776, 123 );
drawCircle( 2197, 776, 133 );
drawCircle( 2197, 776, 143 );
drawCircle( 2330, 776, 3 );
drawCircle( 2330, 776, 13 );
drawCircle( 2330, 776, 23 );
drawCircle( 2330, 776, 33 );
drawCircle( 2330, 776, 43 );
drawCircle( 2330, 776, 53 );
drawCircle( 2330, 776, 63 );
drawCircle( 2330, 776, 73 );
drawCircle( 2330, 776, 83 );
drawCircle( 2330, 776, 93 );
drawCircle( 2330, 776, 103 );
drawCircle( 2330, 776, 113 );
drawCircle( 2330, 776, 123 );
drawCircle( 2330, 776, 133 );
drawCircle( 2330, 776, 143 );
drawCircle( 2330, 776, 153 );
drawCircle( 2330, 776, 163 );
drawCircle( 2330, 776, 173 );
drawCircle( 2330, 776, 183 );
drawCircle( 2330, 776, 193 );
drawCircle( 2330, 776, 203 );
drawCircle( 2330, 776, 213 );
drawCircle( 2463, 776, 3 );
drawCircle( 2463, 776, 13 );
drawCircle( 2463, 776, 23 );
drawCircle( 2463, 776, 33 );
drawCircle( 2463, 776, 43 );
drawCircle( 2463, 776, 53 );
drawCircle( 2463, 776, 63 );
drawCircle( 2463, 776, 73 );
drawCircle( 1000, 909, 3 );
drawCircle( 1000, 909, 13 );
drawCircle( 1000, 909, 23 );
drawCircle( 1000, 909, 33 );
drawCircle( 1000, 909, 43 );
drawCircle( 1000, 909, 53 );
drawCircle( 1133, 909, 3 );
drawCircle( 1133, 909, 13 );
drawCircle( 1133, 909, 23 );
drawCircle( 1133, 909, 33 );
drawCircle( 1133, 909, 43 );
drawCircle( 1266, 909, 3 );
drawCircle( 1266, 909, 13 );
drawCircle( 1266, 909, 23 );
drawCircle( 1266, 909, 33 );
drawCircle( 1399, 909, 3 );
drawCircle( 1399, 909, 13 );
drawCircle( 1399, 909, 23 );
drawCircle( 1399, 909, 33 );
drawCircle( 1399, 909, 43 );
drawCircle( 1532, 909, 3 );
drawCircle( 1532, 909, 13 );
drawCircle( 1532, 909, 23 );
drawCircle( 1532, 909, 33 );
drawCircle( 1532, 909, 43 );
drawCircle( 1665, 909, 3 );
drawCircle( 1665, 909, 13 );
drawCircle( 1665, 909, 23 );
drawCircle( 1665, 909, 33 );
drawCircle( 1665, 909, 43 );
drawCircle( 1798, 909, 3 );
drawCircle( 1931, 909, 3 );
drawCircle( 2064, 909, 3 );
drawCircle( 2197, 909, 3 );
drawCircle( 2330, 909, 3 );
drawCircle( 2330, 909, 13 );
drawCircle( 2463, 909, 3 );
drawCircle( 2463, 909, 13 );
drawCircle( 2463, 909, 23 );
drawCircle( 1000, 1042, 3 );
drawCircle( 1133, 1042, 3 );
drawCircle( 1133, 1042, 13 );
drawCircle( 1266, 1042, 3 );
drawCircle( 1399, 1042, 3 );
drawCircle( 1532, 1042, 3 );
drawCircle( 1665, 1042, 3 );
drawCircle( 1798, 1042, 3 );
drawCircle( 1931, 1042, 3 );
drawCircle( 2064, 1042, 3 );
drawCircle( 2197, 1042, 3 );
drawCircle( 2330, 1042, 3 );
drawCircle( 2330, 1042, 13 );
drawCircle( 2330, 1042, 23 );
drawCircle( 2330, 1042, 33 );
drawCircle( 2330, 1042, 43 );
drawCircle( 2463, 1042, 3 );
drawCircle( 2463, 1042, 13 );
drawCircle( 2463, 1042, 23 );
drawCircle( 2463, 1042, 33 );
drawCircle( 1000, 1175, 3 );
drawCircle( 1133, 1175, 3 );
drawCircle( 1266, 1175, 3 );
drawCircle( 1266, 1175, 13 );
drawCircle( 1399, 1175, 3 );
drawCircle( 1532, 1175, 3 );
drawCircle( 1665, 1175, 3 );
drawCircle( 1665, 1175, 13 );
drawCircle( 1798, 1175, 3 );
drawCircle( 1798, 1175, 13 );
drawCircle( 1931, 1175, 3 );
drawCircle( 1931, 1175, 13 );
drawCircle( 2064, 1175, 3 );
drawCircle( 2064, 1175, 13 );
drawCircle( 2064, 1175, 23 );
drawCircle( 2197, 1175, 3 );
drawCircle( 2197, 1175, 13 );
drawCircle( 2330, 1175, 3 );
drawCircle( 2330, 1175, 13 );
drawCircle( 2463, 1175, 3 );
drawCircle( 2463, 1175, 13 );
drawCircle( 2463, 1175, 23 );
drawCircle( 2463, 1175, 33 );
drawCircle( 2463, 1175, 43 );
drawCircle( 2463, 1175, 53 );
drawCircle( 2463, 1175, 63 );
drawCircle( 1000, 1308, 3 );
drawCircle( 1000, 1308, 13 );
drawCircle( 1000, 1308, 23 );
drawCircle( 1000, 1308, 33 );
drawCircle( 1000, 1308, 43 );
drawCircle( 1133, 1308, 3 );
drawCircle( 1133, 1308, 13 );
drawCircle( 1133, 1308, 23 );
drawCircle( 1133, 1308, 33 );
drawCircle( 1133, 1308, 43 );
drawCircle( 1133, 1308, 53 );
drawCircle( 1133, 1308, 63 );
drawCircle( 1133, 1308, 73 );
drawCircle( 1133, 1308, 83 );
drawCircle( 1133, 1308, 93 );
drawCircle( 1133, 1308, 103 );
drawCircle( 1266, 1308, 3 );
drawCircle( 1266, 1308, 13 );
drawCircle( 1266, 1308, 23 );
drawCircle( 1399, 1308, 3 );
drawCircle( 1399, 1308, 13 );
drawCircle( 1399, 1308, 23 );
drawCircle( 1399, 1308, 33 );
drawCircle( 1399, 1308, 43 );
drawCircle( 1399, 1308, 53 );
drawCircle( 1532, 1308, 3 );
drawCircle( 1532, 1308, 13 );
drawCircle( 1532, 1308, 23 );
drawCircle( 1665, 1308, 3 );
drawCircle( 1665, 1308, 13 );
drawCircle( 1665, 1308, 23 );
drawCircle( 1798, 1308, 3 );
drawCircle( 1798, 1308, 13 );
drawCircle( 1798, 1308, 23 );
drawCircle( 1798, 1308, 33 );
drawCircle( 1931, 1308, 3 );
drawCircle( 1931, 1308, 13 );
drawCircle( 1931, 1308, 23 );
drawCircle( 2064, 1308, 3 );
drawCircle( 2064, 1308, 13 );
drawCircle( 2064, 1308, 23 );
drawCircle( 2064, 1308, 33 );
drawCircle( 2197, 1308, 3 );
drawCircle( 2197, 1308, 13 );
drawCircle( 2197, 1308, 23 );
drawCircle( 2197, 1308, 33 );
drawCircle( 2330, 1308, 3 );
drawCircle( 2330, 1308, 13 );
drawCircle( 2330, 1308, 23 );
drawCircle( 2330, 1308, 33 );
drawCircle( 2330, 1308, 43 );
drawCircle( 2463, 1308, 3 );
drawCircle( 2463, 1308, 13 );
drawCircle( 2463, 1308, 23 );
drawCircle( 2463, 1308, 33 );
drawCircle( 1000, 1441, 3 );
drawCircle( 1000, 1441, 13 );
drawCircle( 1000, 1441, 23 );
drawCircle( 1000, 1441, 33 );
drawCircle( 1133, 1441, 3 );
drawCircle( 1133, 1441, 13 );
drawCircle( 1133, 1441, 23 );
drawCircle( 1133, 1441, 33 );
drawCircle( 1266, 1441, 3 );
drawCircle( 1266, 1441, 13 );
drawCircle( 1266, 1441, 23 );
drawCircle( 1399, 1441, 3 );
drawCircle( 1399, 1441, 13 );
drawCircle( 1399, 1441, 23 );
drawCircle( 1532, 1441, 3 );
drawCircle( 1532, 1441, 13 );
drawCircle( 1532, 1441, 23 );
drawCircle( 1665, 1441, 3 );
drawCircle( 1665, 1441, 13 );
drawCircle( 1665, 1441, 23 );
drawCircle( 1798, 1441, 3 );
drawCircle( 1798, 1441, 13 );
drawCircle( 1931, 1441, 3 );
drawCircle( 1931, 1441, 13 );
drawCircle( 2064, 1441, 3 );
drawCircle( 2064, 1441, 13 );
drawCircle( 2197, 1441, 3 );
drawCircle( 2197, 1441, 13 );
drawCircle( 2330, 1441, 3 );
drawCircle( 2463, 1441, 3 );
drawCircle( 1000, 1574, 3 );
drawCircle( 1133, 1574, 3 );
drawCircle( 1266, 1574, 3 );
drawCircle( 1399, 1574, 3 );
drawCircle( 1532, 1574, 3 );
drawCircle( 1665, 1574, 3 );
drawCircle( 1798, 1574, 3 );
drawCircle( 1931, 1574, 3 );
drawCircle( 2064, 1574, 3 );
drawCircle( 2197, 1574, 3 );
drawCircle( 2330, 1574, 3 );
drawCircle( 2463, 1574, 3 );
drawCircle( 1000, 1707, 3 );
drawCircle( 1133, 1707, 3 );
drawCircle( 1266, 1707, 3 );
drawCircle( 1399, 1707, 3 );
drawCircle( 1532, 1707, 3 );
drawCircle( 1665, 1707, 3 );
drawCircle( 1798, 1707, 3 );
drawCircle( 1931, 1707, 3 );
drawCircle( 2064, 1707, 3 );
drawCircle( 2197, 1707, 3 );
drawCircle( 2330, 1707, 3 );
drawCircle( 2463, 1707, 3 );
drawCircle( 1000, 1840, 3 );
drawCircle( 1133, 1840, 3 );
drawCircle( 1133, 1840, 13 );
drawCircle( 1133, 1840, 23 );
drawCircle( 1266, 1840, 3 );
drawCircle( 1399, 1840, 3 );
drawCircle( 1532, 1840, 3 );
drawCircle( 1665, 1840, 3 );
drawCircle( 1798, 1840, 3 );
drawCircle( 1931, 1840, 3 );
drawCircle( 2064, 1840, 3 );
drawCircle( 2197, 1840, 3 );
drawCircle( 2330, 1840, 3 );
drawCircle( 2463, 1840, 3 );
drawCircle( 1000, 1973, 3 );
drawCircle( 1133, 1973, 3 );
}
