// states
#define STATE_STARTUP 0
#define STATE_SEARCHING 1
#define STATE_CARRYING 2
#define STATE_RETURNING 3


//IR sensor
#define IR_R_PIN A3
#define IR_L_PIN A1
#define IR_F_PIN A0
#define IR_R_MODEL 1080
#define IR_L_MODEL 1080
#define IR_F_MODEL 20150

 
// Ultrasonic sensor

#define SONIC_L_TRIGGER_PIN  9
#define SONIC_L_ECHO_PIN     10
#define SONIC_MAX_DISTANCE   500
#define SONIC_R_TRIGGER_PIN  0
#define SONIC_R_ECHO_PIN     1

//Hall effect sensor
#define HALL_SENSOR_FLIP_PIN 13
#define HALL_SENSOR_NO_FLIP_PIN 14

/*
 *  config parameters
 */

// drive settings

#define STANDARD_VEL 5
#define POWER_L_TO_R 0.97
#define MAX_VEL 12
#define WHEEL_SEPARATION 25
#define VEL2DIS_TIME_MULT 1240
#define CMS2POWER 20

//mine number limit
#define MINE_CENTERED_MAX 20
#define MINE_CENTERED_MIN 5
#define MINE_MAX_RANGE 40
