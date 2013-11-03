
//pin defs
#define lcd_tx	14
#define lcd_rx	15
#define hymera_reprog_tx	16
#define hymera_reprog_rx	17
#define telit_tx	18
#define telit_rx	19
#define temp_sense	11
#define gsm_shield rx	10
#define gsm_power	5
#define telit_power_fet	4
#define gsm_tx	3
#define gps_tx	1
#define gps_rx	0
#define current_sense_1	A5
#define current_sense_2	A6
#define batt_sense	A4
#define relay_cs	47
#define mega_168_cs	49
#define sd_cs	SS

#define log_file "log4.csv"

long int last_send  = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("started");

  setup_rtc();
  setup_temp();
  setup_sd();
  setup_hymera();

//  pinMode(gsm_power_fet,OUTPUT);
   pinMode(sd_cs,OUTPUT);
   pinMode(relay_cs,OUTPUT);
   digitalWrite(sd_cs,HIGH);
   digitalWrite(relay_cs,HIGH);
  
  //dump_log();
  //log();
}

void loop()
{
  //respond to serial commands
  if(Serial.available())
  {
    char c = Serial.read();
    switch(c)
    {
      case 'd':
        dump_log();
        break;
      case 'l':
        log();
        break;
      case 'h':
        Serial.println(fetch_hymera_data());
        break;
      default:
        Serial.print("unknown command");
        Serial.println(c);
        break;
    }
  }
  
  if( millis() > last_send + 60000 )
  {
    last_send = millis();
    log();
  }  
    
  delay(1000);
  print_time();
}

void log()
{
  Serial.println("logging");
  //get the data
  float batt_sense = analogRead(batt_sense)/70; //fix ratio
  float temp = get_temp()/100;
  float uptime = millis()/1000;
  int hymera_data = fetch_hymera_data();
  
  //create a string with the stuff we're logging and write to sd
  String log_string;
  log_string += uptime;
  log_string += ",";
  log_string += batt_sense;
  log_string += ",";
  log_string += temp;
  log_string += ",";
  log_string += hymera_data;
  write_log(log_string);
  
  //send the same stuff to xively
  setup_gsm();
  sendData(uptime,batt_sense,temp);
  close_connection();
  //print_client_msg();
}



