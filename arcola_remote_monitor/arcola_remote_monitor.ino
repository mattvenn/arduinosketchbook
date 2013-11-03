#define relay_cs 47
#define sd_cs SS
#define gsm_power 5 // for gsm shield, but we don't control it, the library does
#define gsm_power_fet 4 //for telit

#define batt_sense A4
#define temp_sense 11

#define log_file "log4.csv"

long int last_send  = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("started");

  setup_rtc();
  setup_temp();
  setup_sd();


//  pinMode(gsm_power_fet,OUTPUT);
   pinMode(sd_cs,OUTPUT);
   pinMode(relay_cs,OUTPUT);
   digitalWrite(sd_cs,HIGH);
   digitalWrite(relay_cs,HIGH);
  
  //dump_log();
  log();
}

void loop()
{
  if( millis() > last_send + 60000 )
  {
    last_send = millis();
    log();
  }  
  delay(500);
  print_time();
}

void log()
{
  Serial.println("logging");
  //get the data
  float batt_sense = analogRead(batt_sense)/70; //fix ratio
  float temp = get_temp()/100;
  float uptime = millis()/1000;

  //create a string with the stuff we're logging and write to sd
  String log_string;
  log_string += uptime;
  log_string += ",";
  log_string += batt_sense;
  log_string += ",";
  log_string += temp;
  write_log(log_string);
  
  //send the same stuff to xively
  setup_gsm();
  sendData(uptime,batt_sense,temp);
  close_connection();
  //print_client_msg();
}



