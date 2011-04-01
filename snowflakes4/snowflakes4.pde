
/*
500 iterations in 10 seconds
 checksum: 126405.55436355762
 
 */

//import controlP5.*;
//ControlP5 controlP5;
//Slider alphaSlider, betaSlider;
int iterations = 0;    

//    int Ycells = 200;

boolean benchMark = true;
int benchMarkIterations = 1000;
boolean debug = false;
int debugIterations = 100;
int Xcells = 100;
int Ycells = Xcells; //200; //int( 200 * 1.156 );

snowcell[][] snowArray;
//float startDiffusionMass = 0.65;
int flakeWidth = 0; //global for how wide the snowflake is

float cellWidth = 4.5;
float cellHeight;
int startX = 0; //Xcells / 2;
int startY = 0; //Ycells / 2;

PFont fontA;
void setup() 
{ 
if( debug )
{
    cellWidth = 35;
    Xcells = 20;
    Ycells =10;
}
  cellHeight = cellWidth / 2 * tan( radians( 60 ) ); 

  size(int(Xcells * cellWidth * 2), int(Ycells * cellHeight * 2), P2D);
  rectMode( CENTER );
  fontA = loadFont("AlBattar-10.vlw");

  // Set the font and its size (in units of pixels)
  textFont(fontA, 15 );

  //  noLoop();
  noStroke();

  snowArray = new snowcell[Xcells][Ycells]; 

  println( "init snowcell array" );
  for( int y = 0; y < Ycells ; y ++ )
  {
    for( int x = 0; x < Xcells; x ++ )
    {
      if( inCalcSpace( x, y ) )
      snowArray[x][y] = new snowcell(x,y,Xcells,Ycells,snowArray);
    }
  }
   for( int y = 0; y < Ycells ; y ++ )
  {
    for( int x = 0; x < Xcells; x ++ )
    {
      if( inCalcSpace( x, y ) )
      snowArray[x][y].getNeighbours();
    }
  }
  

  //seed
  //  snowArray[Xcells/2][Ycells/2].setState();
  snowArray[0][0].setState();
  println( "done" );

  if( benchMark )
  {
    double checkSum = 0;
    for( int x = startX ; x < Xcells ; x ++ )
    {
      for( int y = startY; y < Ycells ; y ++ )
      {
        if( inCalcSpace( x, y ))
        {
        snowcell cell = snowArray[x][y];
        checkSum += cell.getCheckSum();
        }
      }
    }
    println( "checksum: " + checkSum );
    drawIt2();
  }

  /*  controlP5 = new ControlP5(this);
   controlP5.Label label;
   int xInit = width - 100;
   int xSpace = 20;
   int xSpacing = 40;
   int yInit = 10;
   int slidwidth = 20;
   int slidheight = int(height * 0.8 );
   
   alphaSlider = controlP5.addSlider("alphaConst",0,0.003,0.001,xInit + xSpace,yInit,slidwidth,slidheight);
   label = alphaSlider.captionLabel();
   label.set( "a" );
   xSpace += xSpacing; 
   
   betaSlider = controlP5.addSlider("betaConst",0,0.01,0,xInit + xSpace,yInit,slidwidth,slidheight);
   label = betaSlider.captionLabel();
   label.set( "b" );
   xSpace += xSpacing; 
   
   */
} 



void draw()
{

  if( roomToGrow() )
  {
    background(0);
    //controlP5.draw() ;

    iterations ++;
    if( iterations % 100 == 0 )
      println( iterations );

    for( int x = startX ; x < Xcells ; x ++ )
    {
      for( int y = startY; y < Ycells ; y ++ )
      {
       if( inCalcSpace(x, y ) )
        {
          snowcell cell = snowArray[x][y];
          if( ! cell.getState() )
          {
            cell.updateStatus();
            cell.doDiffusion();  
            cell.doFreezing();   
            cell.doAttachment();
            cell.doMelting();
          }
        }
      }
    }

    for( int x = startX ; x < Xcells ; x ++ )
    {
      for( int y = startY; y < Ycells ; y ++ )
      { 
        if( inCalcSpace(x, y ) )
        {
          snowcell cell = snowArray[x][y];

          cell.updateState();
        }
      }
    }

    drawIt2();
    //    if( iterations % 10 == 0 )
    //    saveFrame( "snow-####.png" );
  }
  else
  {
    noLoop();
    println( "finished" ); 
    println( iterations + " iterations in " + millis() / 1000 + " seconds" );
    if( benchMark )
    {
      double checkSum = 0;
      for( int x = 0 ; x < Xcells ; x ++ )
      {
        for( int y = 0; y < Ycells ; y ++ )
        {
          if( inCalcSpace( x, y ))
          {
          snowcell cell = snowArray[x][y];
          checkSum += cell.getCheckSum();
          }
        }
      }
      println( "checksum: " + checkSum );
    }
  }
}

void drawCell( int x, int y )
{


  snowcell cell = snowArray[x][y];
  float colour = 0;
    if(keyPressed) 
   {
   if (key == 'd') 
   colour = 100 * cell.getDiffusionMass();
   if( key == 'b' )
   colour = 100 * cell.getBoundaryMass();
   if( key == 'c' )
   colour = 100 * cell.getCrystalMass();
   if( key == 'a' && cell.getState() )
   colour = 255;
   if( key == 'B' && cell.getBoundary() )
   colour = 255;
   }
   else
   {
  if( cell.getState() )
  {
    colour = 150 * cell.getCrystalMass();
  }
  else
  {
    colour = 100 * cell.getDiffusionMass();
  }
   }

   fill( colour );

  
  rect( 0,0,cellWidth,cellHeight );
  fill( 255 );
//  text(cell.getAttachedNeighbours(), 0, 0);  
if( debug )
  text(cell.numNeighbours, 0, 0);  
}


void drawIt2()
{

 
if( ! debug ){  
      translate( width /2, height / 2 );
  for( int i = 0; i < 6 ; i ++ )
  {
pushMatrix();
  rotate( radians( 60 * i ) );

      drawIt();
      popMatrix();
  }
}
else
{
  drawIt();
}
}
//draw them
void drawIt()
{

//  translate( 0, cellHeight);
  for( int y = 0; y < Ycells ; y ++ )
  {    
    pushMatrix();
    if( y % 2 == 0 )
      translate( cellWidth / 2, 0 );

   // translate( cellWidth / 2, 0 );
    for( int x = 0 ; x < Xcells ; x ++ )
    {
      if( inCalcSpace(x,y) )
      {
        
        drawCell(x,y)        ;
        if( ! debug )
        {
        pushMatrix();
        
        translate(0, (-y * 2 * cellHeight )); //4.33025 )); //0, -1 * cellHeight * y);
                drawCell(x,y);
        popMatrix();
        }
      }
      translate( cellWidth, 0 );
    }
    popMatrix();
    translate( 0, cellHeight);
  }
}

boolean inCalcSpace( int x, int y )
{
 // println( "x: " + x + " y: " + y + " x tan30= " + x * tan(radians( 30 ) ) );
  if( y <= x * tan( radians( 36 )))
    return true;
  return false;
}

int getYFromX( int x )
{
  return (int)floor( (x * tan( radians( 30 ))));
}
int getXFromY( int y )
{
  return (int)floor((y / tan( radians( 30 ))));
}

//don't know how to work this out yet.
boolean roomToGrow()
{


  int startX = Xcells;
  // if( benchMark && iterations > 0 && ( iterations % 500 == 0 ) )
  //    return false;
  /*
  if( iterations % 50 == 0 )
  {
    for( int i = 0; i < Xcells ; i ++ )
    {
      snowcell testCell = snowArray[i][0];     
      if( testCell.getState() && i < startX )
      {
        startX = i;
        break;
      }
    }
    flakeWidth = Xcells - 2 * startX;
    if( flakeWidth < 5 )
      flakeWidth = 5;
  }
*/
 
if( debug  && iterations > debugIterations )
  return false;    
 
 if( benchMark && iterations > benchMarkIterations  ) 
 return false;
 
  snowcell testCell = snowArray[Xcells - Xcells/10][0];
  return ! testCell.getState();
 
  
}

