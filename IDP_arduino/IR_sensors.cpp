#include <Arduino.h>
#include "SharpIR.h"
#include "config.h"
#include <HCSR04.h>

//get wall & mine position
int F_wall_distance = 100, L_wall_distance = 100, R_wall_distance = 100, L_mine_distance = 100, R_mine_distance = 100, hall_1 = 0, hall_2 = 0, hall_3 = 0;
bool mine_centered = false, mine_L = false, mine_R = false;
bool magnet_direction_flip = false, magnet_detection = false, magnet_side = false;

//sensor variables
UltraSonicDistanceSensor sonar_L(SONIC_L_TRIGGER_PIN, SONIC_L_ECHO_PIN);
UltraSonicDistanceSensor sonar_R(SONIC_R_TRIGGER_PIN, SONIC_R_ECHO_PIN);

SharpIR IRsensor_L = SharpIR(IR_L_PIN, IR_L_MODEL);
SharpIR IRsensor_R = SharpIR(IR_R_PIN, IR_R_MODEL);
SharpIR IRsensor_F = SharpIR(IR_F_PIN, IR_F_MODEL);

void sensors_init() {
  pinMode(SONIC_L_TRIGGER_PIN, OUTPUT);
  pinMode(SONIC_L_ECHO_PIN, INPUT);
  
  pinMode(SONIC_R_TRIGGER_PIN, OUTPUT);
  pinMode(SONIC_R_ECHO_PIN, INPUT);
  
  pinMode(IR_L_PIN, INPUT);
  pinMode(IR_R_PIN, INPUT);
  pinMode(IR_F_PIN, INPUT);
  
  pinMode(HALL_SENSOR_1_PIN, INPUT);
  pinMode(HALL_SENSOR_2_PIN, INPUT);
  pinMode(HALL_SENSOR_3_PIN, INPUT);
}

void hall_reset() {
  magnet_direction_flip = false, magnet_detection = false, magnet_side = false;
}

void get_hall_sensors() {
  hall_1 = analogRead(HALL_SENSOR_1_PIN);
  hall_2 = analogRead(HALL_SENSOR_2_PIN);
  hall_3 = analogRead(HALL_SENSOR_3_PIN);
  if(digitalRead(HALL_DETECTED_PIN) == HIGH) {
    magnet_detection = true;
  }
  if(digitalRead(HALL_FLIP_PIN) == HIGH) {
    magnet_direction_flip = true;
  }
  // TODO: fix comparisons and tresholds - TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if(hall_1 > 560 or hall_2 > 560 or hall_3 > 560 or
     hall_1 < 460 or hall_2 < 460 or hall_3 < 460) { // There is a magnet on top
    magnet_side = true;
  }
}

//wall position
void get_wall_position(){
  int l_wall_dist, r_wall_dist;
  l_wall_dist = sonar_L.measureDistanceCm();
  r_wall_dist = sonar_R.measureDistanceCm();
  
  if (l_wall_dist >= 7){
    L_wall_distance = l_wall_dist;
  } else if (r_wall_dist >= 7){
    R_wall_distance = r_wall_dist;
  }
  
  F_wall_distance = IRsensor_F.distance();
}

//mine position
void get_mine_position(){
  L_mine_distance = IRsensor_L.distance();
  R_mine_distance = IRsensor_R.distance();
  mine_centered = false, mine_L = false, mine_R = false;

  if (L_mine_distance > MINE_CENTERED_MIN and L_mine_distance < MINE_CENTERED_MAX and
      R_mine_distance > MINE_CENTERED_MIN and R_mine_distance < MINE_CENTERED_MAX){
    if (abs(L_mine_distance - R_mine_distance) <= 2)
      mine_centered = true;
    else if (L_mine_distance > R_mine_distance)
      mine_L = true;
    else if (L_mine_distance < R_mine_distance)
      mine_R = true;
  } else if (L_mine_distance < MINE_MAX_RANGE and F_wall_distance > L_mine_distance and R_wall_distance + 20 > L_mine_distance){
    mine_L = true;
  } else if (R_mine_distance < MINE_MAX_RANGE and F_wall_distance > R_mine_distance and L_wall_distance + 20 > R_mine_distance){
    mine_R = true;
  } 
}
