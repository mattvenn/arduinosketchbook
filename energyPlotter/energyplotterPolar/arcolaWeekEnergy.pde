#ifdef ARCOLA_WEEK_CIRCLES
int minRadius = 20; //steps not mm
int lastSegment = 0;
boolean newCell = false;
int radius = minRadius;
int radiusIncrement = 20; //steps not mm
int energyPerSegment = 100; //how much energy (KWS) before we draw a segment. There are 20 segments to a circle
/*
with 50kws per segment that makes each circle worth 1000kws, or 0.3kwh, 
as the max our house seems to use is about 16kwh per hour, 1 set of circles (which is 10 minutes) would be 2.6kwh per 10 minutes
so this would result in about 9 circles for high energy times
*/
int remainderEnergy;
int lastHour = -1;

//takes an energy and a minute
void drawEnergy( float energy, int day, int hour )
{
   //sanity check
   if( day < 0 || day > 6 ) 
        {
          Serial.print( "bad day " );
          Serial.println( day );
          return;
        }
        if( energy < 0 || energy > MAX_ENERGY )
        {
          Serial.print( "bad energy " );
          Serial.println( energy );
          return;
        }
        if( hour < 0 || hour > 23 )
        {
          Serial.print( "bad hour " );
          Serial.println( hour );
          return;
        }
  
  if( hour != lastHour)
  {
    newCell = true;
    radius = minRadius;
    lastSegment = 0;
    lastHour = hour;

  }
  else
  {
    newCell = false;
  }
 
  int y = day;
  int x = hour;
  int cellWidth = ( w - 2 * margin ) / 24;
  x *= cellWidth;
  x += cellWidth;
  x += margin;

  int cellHeight = ( h - 2 * margin ) / 7;
  y *= cellHeight;
  y += cellHeight;
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
