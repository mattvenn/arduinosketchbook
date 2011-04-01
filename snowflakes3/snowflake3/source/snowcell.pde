class snowcell
{
  int X,Y;
  float diffusionMass = 0.5;
  float crystalMass = 0;
  float boundaryMass = 0;

  float newDiffusionMass = 0;
  float newCrystalMass = 0;
  float newBoundaryMass = 0;
  boolean newState = false;

  //constants

  float b = 1.3;  //keep above 1
  float alpha = 0.08;
  float theta = 0.025;
  float k = 0.003;
  float u = 0.07;
  float y = 0.00005;

  
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
  void doDiffusion()
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
  void doFreezing()
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
  void doAttachment()
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
  void doMelting()
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

  void updateStatus()
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


  void setState()
  {
    state = true;
    diffusionMass = 0;
     boundaryMass = 0;
    crystalMass = 1;
    boundary = false;
  }

  boolean getState()
  {
    return state;
  }

  boolean getBoundary()
  {
    return boundary;
  }

  float getDiffusionMass()
  {
    return diffusionMass;
  }
  float getCrystalMass()
  {
    return crystalMass;
  }
  float getBoundaryMass()
  {
    return boundaryMass;
  }


  void printState()
  {
    print( "x:" + nf( X, 3 ) + " y:" + nf( Y, 3 ) + " df: " + nf( diffusionMass, 1, 2 ) );
    print( " state:" + state );
    println("");
  }

  void getNeighbours()
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



