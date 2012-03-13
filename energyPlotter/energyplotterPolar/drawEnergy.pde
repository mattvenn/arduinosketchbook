#ifdef DRAW_ENERGY_CIRCLES
int minRadius = 20; //steps not mm
int lastSegment = 0;
boolean newCell = false;
int radius = minRadius;
int radiusIncrement = 10; //steps not mm
int energyPerSegment = 50; //how much energy (KWS) before we draw a segment. There are 20 segments to a circle
/*
with 50kws per segment that makes each circle worth 1000kws, or 0.3kwh, 
as the max our house seems to use is about 16kwh per hour, 1 set of circles (which is 10 minutes) would be 2.6kwh per 10 minutes
so this would result in about 9 circles for high energy times
*/
int remainderEnergy;
int lastMinute;

//takes an energy and a minute
void drawEnergy( float energy, int minute )
{
   //sanity check
   if( minute < 0 || minute >= 1440 ) 
        {
          Serial.print( "bad minute " );
          Serial.println( minute );
          return;
        }
        if( energy < 0 || energy > 1000 )
        {
          Serial.print( "bad energy " );
          Serial.println( energy );
          return;
        }
  minute = minute / 10;
  if( minute != lastMinute)
  {
    newCell = true;
    radius = minRadius;
    lastSegment = 0;
    lastMinute = minute;

  }
  else
  {
    newCell = false;
  }
 
  int y = minute / 12;
  int x = minute % 12;
  int cellWidth = ( w - 2 * margin ) / 13;
  x *= cellWidth;
  x += cellWidth;
  x += margin;

  y *= cellWidth;
  y += cellWidth;
  y += ceiling;
  int r = cellWidth / 2;
  
  energy += remainderEnergy;
  int numSegments = (int)(energy / energyPerSegment);
  remainderEnergy = energy - (energyPerSegment * numSegments);
  Serial.print( "segments: " );
  Serial.print( numSegments );
  Serial.print( ", remainder: " );
  Serial.print( remainderEnergy );
  Serial.print( ", radius: " );
  Serial.println( radius );
  
  //only move and draw if we need to, to reduce noise in the night time
  if(numSegments > 0 )
  {
  if( newCell )
  {
    moveTo( x, y );
    Serial.println( "---> moved to" );
  }
  
  drawSegments( x, y, numSegments );
  }
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
#endif
