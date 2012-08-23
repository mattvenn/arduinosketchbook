void moveTo(int x2, int y2) {

  // Turn the stepper motors to move the marker from the current point (x1,
  // y1) to (x2, y2)
  // Note: This only moves in a perfectly straight line if 
  // the distance is the same in both dimensions; this should be fixed, but it
  // works well
  
  /*
  if( x2 == x1 && y2 == y1 )
  {
    return;
  }
  else
  {
    Serial.println( "moving" );
    Serial.println( x2 ); 
    Serial.println( y2 );
  }
  */
  // a2 and b2 are the final lengths of the left and right strings
  int a2 = sqrt(pow(x2,2)+pow(y2,2));
  int b2 = sqrt(pow((w-x2),2)+pow(y2,2));
  //Serial.print( "a2="); Serial.println(a2);
  //Serial.print( "b2="); Serial.println(b2);
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
      stepLeft( stepA );
    //leftStepper.moveTo( leftStepper.currentPosition() + stepA );
    }
    if (b1!=b2) { 
      b1 += stepB;
    //rightStepper.moveTo( rightStepper.currentPosition() + stepB );
     stepRight(stepB);
    }
    //move them
/*
    while( leftStepper.distanceToGo() || rightStepper.distanceToGo() )
    {
      leftStepper.run();
      rightStepper.run();
    }*/
  }
 
  x1 = x2;
  y1=y2;

}
void drawLine(float x, float y, float fx, float fy )
{
  float cx = x + ( (fx - x) / 2 );
  float cy = y + ( (fy - y )/2 );
  drawCurve( x, y, fx, fy, cx, cy );
}

void drawCurve(float x, float y, float fx, float fy, float cx, float cy) {
  // Draw a Quadratic Bezier curve from (x, y) to (fx, fy) using control pt
  // (cx, cy)
  float xt=0;
  float yt=0;
  float lastXt = 0; float lastYt =0;
  for (float t=0; t<=1; t+=.0025) {
    xt = pow((1-t),2) *x + 2*t*(1-t)*cx+ pow(t,2)*fx;
    yt = pow((1-t),2) *y + 2*t*(1-t)*cy+ pow(t,2)*fy;
    /*
    Serial.print( xt );
    Serial.print( "," );
    Serial.println( yt );
    */
    if( abs(lastXt - xt) > 1 || abs(lastYt - yt ) > 1 )
    {
   //   Serial.println( "moved" );
      moveTo(xt, yt);
      lastXt = xt; lastYt = yt;
    }

  }  
}


