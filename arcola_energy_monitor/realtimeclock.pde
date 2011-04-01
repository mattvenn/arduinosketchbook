
int getSeconds()
{
  return RTC.get(DS1307_SEC,true);//read seconds
}

void getDateTime()
{
  dateString = "";
  int hour,minute,sec,year,month,day;
  hour = RTC.get(DS1307_HR,true);  //read the hour and also update all the values by pushing in true
  if( hour < 10 ) dateString.append("0") ;
  dateString.append(hour);
  dateString.append(":");
  minute = RTC.get(DS1307_MIN,false);//read minutes without update (false)
  if( minute < 10 ) dateString.append("0") ;
  dateString.append(minute);
  dateString.append(":");
  sec = RTC.get(DS1307_SEC,false);//read seconds
  if( sec < 10 ) dateString.append("0") ;
  dateString.append(sec);
  dateString.append(",");

  day = RTC.get(DS1307_DATE,false);//read date
  if( day < 10 ) dateString.append("0") ;
  dateString.append(day);
  dateString.append("/");

  month = RTC.get(DS1307_MTH,false);//read date
  if( month < 10 ) dateString.append("0") ;
  dateString.append(month);
  dateString.append("/");

  year = RTC.get(DS1307_YR,false); //read year 
  dateString.append(year); 

/*
formats a string like " 0x36C77319" that is used to date stamp the files on the usb stick
*/
   //vdip1 year 0 == 1980
   year -= 1980;
   //vdip1 sec is 0->30
   sec /= 2;
   unsigned int datecalc=(year<<9)|(month<<5)|(day);
   unsigned int lsdatecalc=(hour<<11)|(minute<<5)|(sec/2);
   char buffer[ 14 ];
   sprintf( buffer, " 0x%X%X", datecalc,lsdatecalc );
   dateAsHexString = buffer;
   #ifdef DEBUG_2
   Serial.print( "hex date string:" );
   Serial.println( dateAsHexString );
   #endif
}

/*
void getDate()
{
  dateString = "";
  dateString.append(RTC.get(DS1307_DATE,true));//read date
  dateString.append("-");
  dateString.append(RTC.get(DS1307_MTH,false));//read month
  dateString.append("-");
  dateString.append(RTC.get(DS1307_YR,false)); //read year 

}
*/
