#ifdef DRAW_DAY_SPIRAL
//not usable at the moment because the pen moves too slowly and there is a problem with drawing circles...
float centreX = w / 2;
float centreY = h / 2;
float energyR = 14 * StepUnit; //10cm radius
float energyScalingFactor = 100000; //make number smaller for less energy 
void drawEnergy( float energy, int minute )
{
  energyR -= energy / energyScalingFactor;
  if( energyR >= 0 )
  {
  float angle = - minute * ( 360.0 / 60 ); // once per hour now as pen was moving too slowly... 1440.0 ); //1440 minutes in a day
  angle += 180; //start from 12 oclock
  Serial.print( "angle: " ); Serial.println( angle );
  Serial.print( "energyR: " ); Serial.println( energyR );
//  println( angle );
  float opp = energyR * sin( rads((int)angle) );
  float adj = energyR * cos( rads((int)angle) );

  drawLine( x1, y1, (int)(centreX + opp),(int)( centreY + adj) );
  }
}
#endif
