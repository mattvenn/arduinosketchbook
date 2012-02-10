import processing.core.*; 
import processing.xml.*; 

import controlP5.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class snowflakes3 extends PApplet {


/*
   
 
 */


ControlP5 controlP5;
Slider alphaSlider, betaSlider;
int iterations = 0;    

//    int Ycells = 200;

int Xcells = 500;
int Ycells = 500; //int( 200 * 1.156 );
int flakeWidth = 1;
snowcell[][] snowArray;
//float startDiffusionMass = 0.65;


float cellWidth = 1.5f;
float cellHeight;

public void setup() 
{ 
  cellHeight = cellWidth / 2 * 1.7321f; //tan60  
  size(PApplet.parseInt(Xcells * cellWidth), PApplet.parseInt(Ycells * cellHeight), P2D);
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

public void draw()
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

public void drawCell( int x, int y )
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
public void drawItScaled()
{
  //  cellHeight = cellWidth / 2 * 1.7321; //tan60  

  float scaledCellWidth = width / flakeWidth * 0.5f;

  float scaledCellHeight = scaledCellWidth / 2 * 1.7321f; 

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
public void drawIt()
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
public boolean roomToGrow()
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






class snowcell
{
  int X,Y;
  float diffusionMass = 0.5f;
  float crystalMass = 0;
  float boundaryMass = 0;

  float newDiffusionMass = 0;
  float newCrystalMass = 0;
  float newBoundaryMass = 0;
  boolean newState = false;

  //constants

  float b = 1.3f;  //keep above 1
  float alpha = 0.08f;
  float theta = 0.025f;
  float k = 0.003f;
  float u = 0.07f;
  float y = 0.00005f;

  
  int attachedNeighbours = 0;
  boolean state = false;
  boolean boundary = false;
  int width, height;
  snowcell[][] snowArray;
  ArrayList neighbours;

  snowcell(int x, int y, int w, int h, snowcell snowarray[][])
  {
    X = x;
    Y = y;
    width = w;
    height = h;
    snowArray = snowarray;

  }

  //step 1  
  public void doDiffusion()
  {

    if( state == false )
    {  
      newDiffusionMass = 0;
      if( boundary )
      {
        //reflecting boundary conditions     
        for (int i = neighbours.size()-1; i >= 0; i--)
        { 
          snowcell cell = (snowcell) neighbours.get(i);
          //cell.printState();
          if( cell.getState() )
          {
            newDiffusionMass += getDiffusionMass();
          }
          else
          {
            newDiffusionMass += cell.getDiffusionMass();
          }
        }
        //my own diffusion mass
        newDiffusionMass += getDiffusionMass();
        newDiffusionMass /= ( neighbours.size() + 1 );
      }
      else
      {
        //discrete diffusion with uniform weight of 1/7
        for (int i = neighbours.size()-1; i >= 0; i--)
        { 
          snowcell cell = (snowcell) neighbours.get(i);
          //cell.printState();
          newDiffusionMass += cell.getDiffusionMass();
        }
        //my own diffusion mass
        newDiffusionMass += getDiffusionMass();
        newDiffusionMass /= ( neighbours.size() + 1 );
      }
    }
    else
    {
      newDiffusionMass = diffusionMass; //should always be 0?
      if( diffusionMass != 0 )
        println( "diffusion mass not 0!" ) ;
    }
  }

  //step 2
  public void doFreezing()
  {
    diffusionMass = newDiffusionMass;
    if( boundary )
    {
      newBoundaryMass = boundaryMass + ( 1 - k ) * diffusionMass;      
      newCrystalMass = crystalMass + k * diffusionMass;
      //surely this next bit can't be right?
      diffusionMass = 0;
    }
    else
    {
      newBoundaryMass = boundaryMass;
      newCrystalMass = crystalMass;
    }
  }

  //step 3
  public void doAttachment()
  {

    boundaryMass = newBoundaryMass;
    crystalMass = newCrystalMass;
    if( boundary )
    {

      if( attachedNeighbours <= 2 )
      {
        if( boundaryMass > b )
          newState = true;
      }
      else if( attachedNeighbours == 3 )
      {
        if( boundaryMass >= 1 )
        {
          newState = true;
        }
        else
        {
          float summedDiffusion = diffusionMass;
          for (int i = neighbours.size()-1; i >= 0; i--)
          {
            snowcell cell = (snowcell) neighbours.get(i);
            summedDiffusion += cell.getDiffusionMass();
          }

          if( summedDiffusion < theta && boundaryMass >= alpha )
            newState = true;
        }
      }
      else if( attachedNeighbours >= 4 )
      {
        newState = true;
      }

    }
  }

  //step 4
  public void doMelting()
  {
    
    if( boundary )
    {
      diffusionMass = diffusionMass + u * boundaryMass + y * crystalMass;
      crystalMass = ( 1 - y ) * crystalMass;
      boundaryMass = ( 1 - u ) * boundaryMass;
    }
    
    if( newState )
      {
        newState = false;
        state = true;
        crystalMass = boundaryMass + crystalMass;
        diffusionMass = 0;
        boundaryMass = 0;
        boundary = false;
      }
  }

  public void updateStatus()
  {
       //finishing the freeze
   


    if( state == false )
    {
      attachedNeighbours = 0;  
      for (int i = neighbours.size()-1; i >= 0; i--)
      { 
        snowcell cell = (snowcell) neighbours.get(i);
        if( cell.getState() )
        {
          boundary = true;
          attachedNeighbours ++;
        }
      }
    }

    
  }


  public void setState()
  {
    state = true;
    diffusionMass = 0;
     boundaryMass = 0;
    crystalMass = 1;
    boundary = false;
  }

  public boolean getState()
  {
    return state;
  }

  public boolean getBoundary()
  {
    return boundary;
  }

  public float getDiffusionMass()
  {
    return diffusionMass;
  }
  public float getCrystalMass()
  {
    return crystalMass;
  }
  public float getBoundaryMass()
  {
    return boundaryMass;
  }


  public void printState()
  {
    print( "x:" + nf( X, 3 ) + " y:" + nf( Y, 3 ) + " df: " + nf( diffusionMass, 1, 2 ) );
    print( " state:" + state );
    println("");
  }

  public void getNeighbours()
  {
    neighbours = new ArrayList();
    int x = X;
    int y = Y;

    if( x > 1 )
      neighbours.add( snowArray[x-1][y] );
    if( x < width - 1 )
      neighbours.add( snowArray[x+1][y] );
    if( y > 1 )
      neighbours.add( snowArray[x][y-1] );
    if( y < height - 1 )
      neighbours.add( snowArray[x][y+1] );

    if( y % 2 == 0 )
    {
      //odd rows
      if(( x < width - 1 ) && ( y > 1 ))
        neighbours.add( snowArray[x+1][y-1] );
      if(( x < width - 1 ) && ( y < height - 1 ))  
        neighbours.add( snowArray[x+1][y+1] ); 
    }
    else

    {
      if(( x > 1 ) && ( y > 1 ))  
        neighbours.add( snowArray[x-1][y-1] );
      if(( x > 1 ) && ( y < height - 1 ))  
        neighbours.add( snowArray[x-1][y+1] );
    }
  }

 
}




  static public void main(String args[]) {
    PApplet.main(new String[] { "--present", "--bgcolor=#666666", "--hide-stop", "snowflakes3" });
  }
}
