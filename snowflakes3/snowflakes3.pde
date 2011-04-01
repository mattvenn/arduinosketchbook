
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
int Xcells = 400;
int Ycells = 400; //int( 200 * 1.156 );

snowcell[][] snowArray;
//float startDiffusionMass = 0.65;


float cellWidth = 2.50;
float cellHeight;
int startX = 0; //Xcells / 2;
int startY = 0; //Ycells / 2;
void setup() 
{ 
  cellHeight = cellWidth / 2 * 1.7321; //tan60  
  size(int(Xcells * cellWidth), int(Ycells * cellHeight), P2D);
  rectMode( CENTER );

  //  noLoop();
  noStroke();
  
  snowArray = new snowcell[Xcells][Ycells]; 

  println( "init snowcell array" );
  for( int i = 0; i < Xcells ; i ++ )
  {
    for( int j = 0; j < Ycells; j ++ )
    {
      snowArray[i][j] = new snowcell(i,j,Xcells,Ycells,snowArray);
    }
  }
  println( "getting neighbours" );
  for( int i = 0; i < Xcells ; i ++ )
  {
    for( int j = 0; j < Ycells; j ++ )
    {
      snowArray[i][j].getNeighbours();
    }
  }

  //seed
  snowArray[Xcells/2][Ycells/2].setState();
//    snowArray[Xcells-1  ][Ycells-1].setState();
  println( "done" );

  if( benchMark )
  {
    double checkSum = 0;
    for( int x = startX ; x < Xcells ; x ++ )
    {
      for( int y = startY; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];
        checkSum += cell.getCheckSum();
      }
    }
    println( "checksum: " + checkSum );
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


boolean parallel = true;
void draw()
{

  if( roomToGrow() )
  {
    background(0);
    //controlP5.draw() ;

    iterations ++;
    if( iterations % 100 == 0 )
      println( iterations );
    if( parallel )
    {
      for( int x = startX ; x < Xcells ; x ++ )
      {
        for( int y = startY; y < Ycells ; y ++ )
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

      for( int x = startX ; x < Xcells ; x ++ )
      {
        for( int y = startY; y < Ycells ; y ++ )
        {
          snowcell cell = snowArray[x][y];

          cell.updateState();
        }
      }
    }    
    drawIt();
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
          snowcell cell = snowArray[x][y];
          checkSum += cell.getCheckSum();
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
  /*  if(keyPressed) 
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
   */
  if( cell.getState() )
  {
    colour = 150 * cell.getCrystalMass();
  }
  else
  {
    colour = 100 * cell.getDiffusionMass();
  }

  fill( colour );
  rect( 0,0,cellWidth,cellHeight );
}

//draw them
void drawIt()
{

  translate( 0, cellHeight/2);
  for( int y = 0; y < Ycells ; y ++ )
  {    
    pushMatrix();
    if( y % 2 == 0 )
      translate( cellWidth / 2, 0 );

    translate( cellWidth / 2, 0 );
    for( int x = 0 ; x < Xcells ; x ++ )
    {
      drawCell(x,y);
      translate( cellWidth, 0 );
    }
    popMatrix();
    translate( 0, cellHeight);
  }
}



//don't know how to work this out yet.
boolean roomToGrow()
{
  if( iterations > benchMarkIterations )
    return false;
  snowcell testCell = snowArray[Xcells - Xcells/10][Ycells/2];
  return ! testCell.getState();
  
}

