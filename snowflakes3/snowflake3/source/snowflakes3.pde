
/*
   
 
 */

import controlP5.*;
ControlP5 controlP5;
Slider alphaSlider, betaSlider;
int iterations = 0;    

//    int Ycells = 200;

int Xcells = 500;
int Ycells = 500; //int( 200 * 1.156 );
int flakeWidth = 1;
snowcell[][] snowArray;
//float startDiffusionMass = 0.65;


float cellWidth = 1.5;
float cellHeight;

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
  println( "done" );

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
    /*    
     for( int x = 0 ; x < Xcells ; x ++ )
     {
     for( int y = 0; y < Ycells ; y ++ )
     {
     snowcell cell = snowArray[x][y];
     cell.doDiffusion();
     cell.doFreezing();
     cell.doAttachment();
     cell.doMelting();
     cell.updateStatus();
     }
     }
     */

    for( int x = 0 ; x < Xcells ; x ++ )
    {
      for( int y = 0; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];
        cell.doDiffusion();
      }
    }

    for( int x = 0 ; x < Xcells ; x ++ )
    {
      for( int y = 0; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];

        cell.doFreezing();
      }
    }

    for( int x = 0 ; x < Xcells ; x ++ )
    {
      for( int y = 0; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];

        cell.doAttachment();
      }
    }

    for( int x = 0 ; x < Xcells ; x ++ )
    {
      for( int y = 0; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];

        cell.doMelting();
      }
    }

    for( int x = 0 ; x < Xcells ; x ++ )
    {
      for( int y = 0; y < Ycells ; y ++ )
      {
        snowcell cell = snowArray[x][y];

        cell.updateStatus();
      }
    } 

    drawIt();
  }
  else
  {
     noLoop();
    println( "finished" ); 
    println( iterations + " iterations in " + millis() / 1000 + " seconds" );
    
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
void drawItScaled()
{
  //  cellHeight = cellWidth / 2 * 1.7321; //tan60  

  float scaledCellWidth = width / flakeWidth * 0.5;

  float scaledCellHeight = scaledCellWidth / 2 * 1.7321; 

  translate( 0, scaledCellHeight/2);
  for( int y = Ycells / 2 - flakeWidth; y < Ycells / 2 + flakeWidth ; y ++ )
  {    
    pushMatrix();
    if( y % 2 == 0 )
      translate( scaledCellWidth / 2, 0 );

    translate( scaledCellWidth / 2, 0 );
    for( int x = Xcells / 2 - flakeWidth ; x < Xcells / 2 + flakeWidth ; x ++ )
    {
      snowcell cell = snowArray[x][y];
      //      cell.printState();
      float colour = cell.getDiffusionMass();
      /*
      if( cell.getState() )
       colour = cell.getCrystalMass();
       colour *= 150;
       */
      fill( colour );
      rect( 0,0,scaledCellWidth,scaledCellHeight );
      translate( scaledCellWidth, 0 );
    }
    popMatrix();
    translate( 0, scaledCellHeight);
  }
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

  int startX = Xcells;
  if( iterations % 50 == 0 )
  {
    for( int i = 0; i < Xcells ; i ++ )
    {
      snowcell testCell = snowArray[i][Ycells/2];     
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

  snowcell testCell = snowArray[Xcells/10][Ycells/2];
  return ! testCell.getState();

}






