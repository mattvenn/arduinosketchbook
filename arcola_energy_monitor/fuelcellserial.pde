//returns 1 if new data has arrived
int updateFuelCell()
{
  for( int i = 0; i < 20 ; i ++ )
    {
  //if data available, update the fuelcell structure
  if (fuelcellSerial.available() > 8 ) 
  {
    #ifdef DEBUG_1
  Serial.println( "got data from fuelcell" );
  #endif

       fuelcell.status = fuelcellSerial.read();
       fuelcell.stackT = fuelcellSerial.read();
       fuelcell.stackV = fuelcellSerial.read();
       fuelcell.stackI = fuelcellSerial.read();
       fuelcell.stackT2 = fuelcellSerial.read();
       fuelcell.stackT3 = fuelcellSerial.read();
       fuelcell.rawcurrent = fuelcellSerial.read() * 256; //high byte
       fuelcell.rawcurrent += fuelcellSerial.read(); //low byte

       //calculate actual current
       fuelcell.current = fuelcell.rawcurrent * 0.00039;
       fuelcell.updated = HIGH;

       return 0;
  }
  delay( 100 );
    }
    //wipe out the old data
    clearFuelCellData();
  return 1;
}

void clearFuelCellData()
{
       fuelcell.status = 0;
       fuelcell.stackT = 0;
       fuelcell.stackV = 0;
       fuelcell.stackI = 0;
       fuelcell.stackT2 = 0;
       fuelcell.stackT3 = 0;
       fuelcell.rawcurrent = 0;
       fuelcell.current = 0;
       fuelcell.updated = LOW;
}

//setup - very simple as there is no flow control or talking required on our behalf - we just listen
void setupFuelCellSerial()
{
 fuelcellSerial.begin(9600); 
}
