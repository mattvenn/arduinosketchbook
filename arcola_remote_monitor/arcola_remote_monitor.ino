#define relay_cs 47
#define sd_cs SS
#define gsm_power 5 // for gsm shield
#define gsm_power_fet 4 //for telit

#define batt_sense A4
#define temp_sense 11

#define log_file "log3.csv"

long int last_send  = 0;


void setup()
{
  Serial.begin(9600);
  Serial.println("started");

  //setup_rtc();
  //setup_temp();
  //setup_sd();
  
  pinMode(gsm_power,OUTPUT);
  digitalWrite(gsm_power,LOW);
  /*
  pinMode(gsm_power_fet,OUTPUT);
  pinMode(sd_cs,OUTPUT);
  pinMode(relay_cs,OUTPUT);
  digitalWrite(sd_cs,LOW);
  digitalWrite(relay_cs,LOW);
*/
  
  //dump_log();
  //log();
  last_send = -60000;
}

void loop()
{
  
   print_client_msg();
  
  
  if( millis() > last_send + 60000 )
  {
    last_send = millis();
    setup_gsm();
  //  sendData(millis());
    print_client_msg();
  close_connection();
// log();
  }  
  Serial.println(millis());
  delay(500);
//  print_time();
//  delay(1000);
  
}

void log()
{
  Serial.println("log started");
  int batt_sense = analogRead(batt_sense);
  //print_time();
  String log_string;
/*
  log_string += get_temp();
  log_string += ",";
*/
  log_string += batt_sense;


  //write_log(log_string);

  //if(setup_gsm())
  {
 //  send_gsm_data(get_temp());
   
   // power_down_gsm();
  }
  
}


