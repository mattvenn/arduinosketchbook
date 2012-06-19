#ifdef DRAW_RANDOMDIRECTION
#define MAXENERGY 500 // think because this is so low, we need to store up all energy used in    10 mins and then draw.
#define MAXLINE halfSquareWidth * StepUnit
#define INFINITY 30000
Point oldPoint, newPoint;

void initDraw()
{
    oldPoint.x = x1;
    oldPoint.y = y1;
    reflected.origin = oldPoint;
    randomSeed(analogRead(A5));
}
//takes an energy and a minute
void drawEnergy( float energy, int minute )
{

     float lineLength = map( energy, 0, MAXENERGY, 0, MAXLINE );
     float angle  =  random( 2 * PI * 100);
     angle /= 100;
     Serial.print( "line length: " ); Serial.println( lineLength );
     Serial.print( "angle: " ); Serial.println( angle );
     Serial.print( "old point x: " ); Serial.print( reflected.origin.x ); Serial.print(  " y: " ); Serial.println( reflected.origin.y );
     reflected.angle = angle;
     reflected.remainder = lineLength;
     while( drawReflectLine( reflected.origin.x, reflected.origin.y, reflected.remainder, reflected.angle ) > 0 )
     {
     }
//    println( "new point x: " + newPoint.x + " y: " + newPoint.y );
    //oldPoint.x = newPoint.x;
    //oldPoint.y = newPoint.y;
}

int drawReflectLine( int x, int y, int lineLength, float angle ) //, int recurse)
{
  int maxXLineLength,maxYLineLength = 0; //maximum length of the line in x and y directions
  //changed from float, will this break it?
  int maxXDist,maxYDist; //maximum room for the line in x and y
  int maxLineLength; //the calculated line length
  int remainder = 0; //leftover line still to draw at the new bounceAngle
  float bounceAngle = 0; //the angle after the line bounces
  Serial.print( "line length: " ); Serial.println( lineLength );

  //ensure angle is within 0 and 2PI
  while( angle < 0 )
  {
    angle += 2 * PI;
  }
  Serial.print( "angle: " ); Serial.println( angle );    

  //depending on the angle of the line, the room we have for it changes. Store this in max[XY]Dist
  if( angle  > (PI * 1.5) || angle < (PI / 2) )
    maxXDist = maxX - x;
  else
    maxXDist = x - minX;
  if( angle > 0&&angle < PI )
    maxYDist = maxY - y;
  else
    maxYDist = y - minY;

  Serial.print( "max X: " ); Serial.print( maxXDist ); Serial.print( ", max Y: " ); Serial.println( maxYDist );
  
  //now work out the actual length of the line we could draw for both X and Y directions
  //divide by 0 doesn't work in C like it does in java, so we have to detect when it happens and assign a high number
  maxXLineLength =  maxXDist / cos(angle);
  maxXLineLength = abs(maxXLineLength);
    if( maxXLineLength == 0 )
        maxXLineLength = INFINITY;
  maxYLineLength = maxYDist / sin(angle);
  maxYLineLength = abs(maxYLineLength);
    if( maxYLineLength == 0 )
        maxYLineLength = INFINITY;

  Serial.print( "max Xdd: " ); Serial.print( maxXLineLength ); Serial.print( ", max Ydd: " ); Serial.println( maxYLineLength );    
  
  //now see which the shortest line is, and we use that one
  //bounce angle is different for each direction
  if( maxXLineLength < maxYLineLength )
  {
    bounceAngle = PI - angle;  
    maxLineLength = maxXLineLength;
  }
  else
  {
    bounceAngle = - angle;
    maxLineLength = maxYLineLength;
  }
  Serial.print( "max line length: " ); Serial.println( maxLineLength );

  //if the lineLength is bigger than the available room, then work out the remainder left to draw
  if( lineLength > maxLineLength )
  {
    remainder = lineLength - maxLineLength;  
    lineLength = maxLineLength;
  }
  Serial.print( "line length: " ); Serial.println( lineLength );

  //we return the coordinates of the end of the line
  Point retPoint;
  retPoint.y = (int)(sin( angle ) * lineLength) + y;
  retPoint.x = (int)(cos( angle ) * lineLength) + x;
  //work out the end points of the line
  Serial.print( "nx: " ); Serial.print( retPoint.x ); Serial.print( " ny: " ); Serial.println( retPoint.y );     

  //draw the line!
  drawLine( x, y, retPoint.x, retPoint.y );
  Serial.println( "drawn line" );
  

  //recurse if necessary to draw the rest of the line
   reflected.origin = retPoint;
   reflected.angle = bounceAngle;
   reflected.remainder = remainder;
    Serial.print( "remainder = " ); Serial.println( remainder );
//    Serial.print( "recurse level: " ); Serial.println( recurse );
//    retPoint = drawReflectLine(retPoint.x,retPoint.y,remainder, bounceAngle ); //, ++recurse); //for y
  
  //otherwise return the final coords
  //println( "returing new point: " + recurse + ":" + retPoint.x + "," + retPoint.y );
    return remainder;

}
#endif

