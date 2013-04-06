//update this to take into account the gondola width
static void FK(int l1, int l2) {

  float a = l1 / config.stepsPerMM;
  float b = config.width - config.gondola_width/config.stepsPerMM;
  float c = l2 /config.stepsPerMM;

  // slow, uses trig
  //float theta = acos((a*a+b*b-c*c)/(2.0*a*b));
  //x = cos(theta)*l1 + limit_left;
  //y = sin(theta)*l1 + limit_top;
  // but we know that cos(acos(i)) = i
  // and we know that sin(acos(i)) = sqrt(1-i*i)
  float i=(a*a+b*b-c*c)/(2.0*a*b);
  x1 = i * l1 + config.gondola_width / 2;
  y1 = sqrt(1.0 - i*i)*l1;
  
}

//don't try to move the pen to a point that is unreachable
boolean validate(int x, int y)
{
  if(x < config.side_margin)
    return false;
  if(x > config.width - config.side_margin)
    return false;
  if(y < config.top_margin)
    return false;
  if(y > config.height)
    return false;
  return true;
}
void moveTo(float x2, float y2) 
{
/*  if(!validate(x2,y2))
  {
    Serial.println("shouldn't see this...");
    Serial.println(x2);
    Serial.println(y2);
    return;
  }
  */
  // a2 and b2 are the final lengths of the left and right strings
 /* int a2 = sqrt(pow(x2,2)+pow(y2,2));
  int b2 = sqrt(pow((w-x2),2)+pow(y2,2));*/
 //take into account the gondola width

  int a2 = sqrt(pow(x2-config.gondola_width/2,2)+pow(y2,2));
  int b2 = sqrt(pow((config.width-x2-config.gondola_width/2),2)+pow(y2,2));

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
    }
    if (b1!=b2) { 
      b1 += stepB;
     stepRight(stepB);
    }

  }
 //calculate the new x, y instead of assuming we're there.
//  FK(a1,b1);
  x1=x2;
  y1=y2;

}
void drawLine(float fx, float fy )
{
  if(!validate(fx,fy))
  {
    Serial.println("not moving there");
    return;
  }

  //control point is on the mid point of delta x and delta y 
  float cx = x1 + ( (fx - x1) / 2 );
  float cy = y1 + ( (fy - y1 )/2 );
  drawCurve( x1, y1, fx, fy, cx, cy );
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


