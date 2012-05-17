#define LOOP_PERIOD 400.0 //seconds
#define MAX_PEN_STEPS 2000
#define MAX_ENERGY 4000 //W
#define stepSpeed 20
#define leftStepDir -1 //these should be set so that the commands l50 and r50 lower the gondola
#define rightStepDir -1
#define LEFT 0
#define RIGHT 1
#define MOTOR_DIST_CM 68
#define DIAMETER 1.24
#define stepsPerRevolution 200
#define PWM_CHANGE_DELAY 1
//pwm is causing arduino to reboot at low values - check with scope
#define PWM_LOW 1
#define PWM_HIGH 55

