int minRadius = 20;
int lastSegment = 0;
boolean newCell = false;
int radius = minRadius;
int radiusIncrement = 10;
int energyPerSegment = 500;
int remainderEnergy;
int lastNumber;

//takes an energy and a minute
void drawEnergy( float energy, int number )
{
  number = number / 10;
  if( number != lastNumber)
  {
    newCell = true;
    radius = minRadius;
    lastSegment = 0;
    lastNumber = number;

  }
  else
  {
    newCell = false;
  }
  //number /= 10;
 
  int y = number / 12;
  int x = number % 12;
  int cellWidth = ( w - 2 * margin ) / 13;
  x *= cellWidth;
  x += cellWidth;
  x += margin;

  y *= cellWidth;
  y += cellWidth;
  y += ceiling;
  int r = cellWidth / 2;

  if( newCell )
  {
    moveTo( x, y );
    Serial.println( "---> moved to" );
  }
  //println( "x: " + x * cellWidth + " y: " + y * cellWidth );
  //int radius = (int)map( energy, 0, maxEnergy, 0, cellWidth / 2 );

  energy += remainderEnergy;
  int numSegments = (int)(energy / energyPerSegment);
  remainderEnergy = energy - (energyPerSegment * numSegments);
  Serial.print( "segments: " );
  Serial.print( numSegments );
  Serial.print( "remainder: " );
  Serial.println( remainderEnergy );

  Serial.print( "radius: " );
  Serial.println( radius );
  drawSegments( x, y, numSegments );
}

//draws a number of increasingly sized circles, segment by segment.
void drawSegments(int x,int y, int numSegments)
{

  int segmentsDrawn = 0;

  int currentSegment = lastSegment;

  Serial.print( "start seg: " );
  Serial.print( currentSegment );
  Serial.print( " num segs: " );
  Serial.println( numSegments );
  while( numSegments > segmentsDrawn )
  {
    if( currentSegment < 20 )
    {
     Serial.print( x );
     Serial.print( "," );
     Serial.print( y );
     Serial.print( ", " );
     Serial.print( radius );
     Serial.print( ", " );
     Serial.print( currentSegment );
     Serial.print( "," );
     Serial.println( currentSegment + 1 );
     drawCircleSegment( x, y, radius, currentSegment, currentSegment + 1 );
     currentSegment +=1;
     segmentsDrawn +=1;

    }
   else
    {
      Serial.println( "reset" );
      radius += radiusIncrement;
      currentSegment = 0;
    }
  }
  lastSegment = currentSegment;
//  println( "segments drawn: " + segmentsDrawn );
  
}

