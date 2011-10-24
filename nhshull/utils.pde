int getAverage( unsigned int val, unsigned int array[], unsigned int arrayLength )
{
  long int total = 0;
  for( int i = 1; i < arrayLength ; i ++ )
  {
    total += array[i];
    array[i-1] = array[i];
  }
  array[arrayLength-1] = val;
  total += val;
  return total /= arrayLength;
}

void initialiseAvgArray( unsigned int array[], unsigned int arrayLength )
{
  for( int i = 0; i < arrayLength ; i ++ )
    array[i] = 0;
}
