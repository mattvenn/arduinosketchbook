struct config_t
{
  float stepsPerMM;
  int hanger_l; 
  int top_margin; 
  int side_margin; 
  float width; 
  float height; 
  int left_limit; //142mm
  int right_limit; //152mm
  byte id;
  float gondola_width; 
  int default_pwm;
  int lowpower_pwm;
  int home_pwm_high;
  int home_pwm_low;
  int home_speed;
  int draw_speed;
  int microstep;
} 
config;
