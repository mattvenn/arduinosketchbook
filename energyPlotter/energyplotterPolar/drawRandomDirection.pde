#ifdef DRAW_RANDOMDIRECTION
#define MAXENERGY 5000
#define MAXLINE 50

Point oldPoint, newPoint;

void initDraw()
{
    oldPoint.x = x1;
    oldPoint.y = y1;
}
//takes an energy and a minute
void drawEnergy( float energy, int minute )
{

     float lineLength = map( energy, 0, MAXENERGY, 0, MAXLINE );
     float angle  =  0; //random( 2 * PI );
     Serial.print( "line length: " ); Serial.println( lineLength );
     Serial.print( "angle: " ); Serial.println( angle );
     Serial.print( "old point x: " ); Serial.print( oldPoint.x ); Serial.print(  " y: " ); Serial.println( oldPoint.y );
      newPoint = drawReflectLine( oldPoint.x, oldPoint.y, (int)lineLength, angle, 0);
    //println( "new point x: " + newPoint.x + " y: " + newPoint.y );
    oldPoint.x = newPoint.x;
    oldPoint.y = newPoint.y;
}

struct Point drawReflectLine( int x, int y, int lineLength, float angle, int recurse)
{
  int maxXLineLength,maxYLineLength = 0; //maximum length of the line in x and y directions
  float maxXDist,maxYDist; //maximum room for the line in x and y
  int maxLineLength; //the calculated line length
  int remainder = 0; //leftover line still to draw at the new bounceAngle
  float bounceAngle = 0; //the angle after the line bounces
  //Serial.print( "line length: " ); Serial.println( lineLength );

  //ensure angle is within 0 and 2PI
  while( angle < 0 )
  {
    angle += 2 * PI;
  }
  //println( "angle: " + angle );    

  //depending on the angle of the line, the room we have for it changes. Store this in max[XY]Dist
  if( angle  > (PI * 1.5) || angle < (PI / 2) )
    maxXDist = maxX - x;
  else
    maxXDist = x - minX;
  if( angle > 0&&angle < PI )
    maxYDist = maxY - y;
  else
    maxYDist = y - minY;

  //Serial.print( "max X: " ); Serial.print( maxXDist ); Serial.print( ", max Y: " ); Serial.println( maxYDist );
  
  //now work out the actual length of the line we could draw for both X and Y directions
  maxXLineLength = (int)abs( maxXDist / cos(angle) );
  //divide by 0 doesn't work in C like it does in java
  if( angle == 0 )
    maxYLineLength = 10000; //infinity
  else
    maxYLineLength = (int)abs( maxYDist /sin(angle) );

  //Serial.print( "max Xdd: " ); Serial.print( maxXLineLength ); Serial.print( ", max Ydd: " ); Serial.println( maxYLineLength );    
  
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
  //Serial.print( "max line length: " ); Serial.println( maxLineLength );

  //if the lineLength is bigger than the available room, then work out the remainder left to draw
  if( lineLength > maxLineLength )
  {
    remainder = lineLength - maxLineLength;  
    lineLength = maxLineLength;
  }
 // Serial.print( "line length: " ); Serial.println( lineLength );

  //work out the end points of the line
  int ny = (int)(sin( angle ) * lineLength) + y;
  int nx = (int)(cos( angle ) * lineLength) + x;
  Serial.print( "nx: " ); Serial.print( nx ); Serial.print( " ny: " ); Serial.println( ny );     

  //draw the line!
  drawLine( x, y, nx, ny );
  Serial.println( "drawn line" );
  
  //we return the coordinates of the end of the line
  Point retPoint;
  retPoint.x = nx;
  retPoint.y = ny;
//  Point retPoint = new Point(nx,ny);
    
  //catch recursion bugs, probably not necessary now the function is working
  if( recurse > 1000 )
  {
    Serial.println( "stopping recursion, broken" );
    return retPoint;
  }

  //recurse if necessary to draw the rest of the line
  if( remainder > 0)
  {
    Serial.print( "remainder = " ); Serial.println( remainder );
    Serial.print( "recurse level: " ); Serial.println( recurse );
    retPoint = drawReflectLine(nx,ny,remainder, bounceAngle, ++recurse); //for y
  }
  
  //otherwise return the final coords
  //println( "returing new point: " + recurse + ":" + retPoint.x + "," + retPoint.y );
  return retPoint;

}
#endif

