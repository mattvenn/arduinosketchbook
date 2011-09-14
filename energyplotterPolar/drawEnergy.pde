int minRadius = 20;
int lastSegment = 0;
boolean newCell = false;
int radius = minRadius;
int radiusIncrement = 10;
float energyPerSegment = 50;
float remainderEnergy;
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
    //Serial.println( "---> moved to" );
  }
  //println( "x: " + x * cellWidth + " y: " + y * cellWidth );
  //int radius = (int)map( energy, 0, maxEnergy, 0, cellWidth / 2 );

  energy += remainderEnergy;
  int numSegments = (int)(energy / energyPerSegment);
  remainderEnergy = energy - (energyPerSegment * numSegments);
  //println( "segments: " + numSegments );
  //println( "remainder: " + remainderEnergy );

  //  println( energy + " = " + radius );
   drawSegments( x, y, numSegments );
}

//draws a number of increasingly sized circles, segment by segment.
void drawSegments(int x,int y, int numSegments)
{

  int segmentsDrawn = 0;

  int currentSegment = lastSegment;

 // println( "start seg: " + currentSegment + " num segs: " + numSegments );
  while( numSegments > segmentsDrawn )
  {
    if( currentSegment < 20 )
    {
   //   println( x + "," + y + ", " + radius + ", " + currentSegment + "," + (currentSegment + 1 ) );
     drawCircleSegment( x, y, radius, currentSegment, currentSegment + 1 );
     currentSegment +=1;
     segmentsDrawn +=1;

    }
   else
    {
   //   println( "reset" );
      radius += radiusIncrement;
      currentSegment = 0;
    }
  }
  lastSegment = currentSegment;
//  println( "segments drawn: " + segmentsDrawn );
  
}

