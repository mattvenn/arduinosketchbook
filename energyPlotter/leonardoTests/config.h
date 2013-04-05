struct config_t
{
  int stepsPerRevolution;
  float stepsPerMM;
  int hanger_l;
  int top_margin;
  int side_margin;
  float motor_dist_mm;
  float w;
  float h;
  byte id;
  float gw;
  int default_pwm;
  int lowpower_pwm;
  int HOME_PWM_HIGH;
  int HOME_PWM_LOW;
  int HOME_SPEED;
  int maxSpeed;
} 
config;
