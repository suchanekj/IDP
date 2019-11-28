// states
#define STATE_STARTUP 0
#define STATE_SEARCHING 1
#define STATE_CARRYING 2
#define STATE_RETURNING 3
#define STATE_FINISHED 4


//IR sensor
#define IR_R_PIN A0
#define IR_L_PIN A1
#define IR_F_PIN A2
#define IR_R_MODEL 1080
#define IR_L_MODEL 1080
#define IR_F_MODEL 20150

 
// Ultrasonic sensor

#define SONIC_L_TRIGGER_PIN  0
#define SONIC_L_ECHO_PIN     1
#define SONIC_MAX_DISTANCE   500
#define SONIC_R_TRIGGER_PIN  2
#define SONIC_R_ECHO_PIN     3

//Hall effect sensor
#define HALL_SENSOR_1_PIN A3
#define HALL_SENSOR_2_PIN A4
#define HALL_SENSOR_3_PIN A5
#define HALL_DETECTED_PIN 6
#define HALL_FLIP_PIN 7

/*
 *  config parameters
 */

// drive settings

#define STANDARD_VEL 10
#define POWER_L_TO_R 0.98
#define MAX_VEL 12
#define WHEEL_SEPARATION 23
#define VEL2DIS_TIME_MULT 640
#define CMS2POWER 20

#define DRIVE_BACKWARD FORWARD
#define DRIVE_FORWARD BACKWARD

//mine number limit
#define MINE_CENTERED_MAX 20
#define MINE_CENTERED_MIN 5
#define MINE_MAX_RANGE 40
