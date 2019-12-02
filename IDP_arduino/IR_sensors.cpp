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


#define NUM_VALUES 7
#define NUM_VALUES_HALL 7
#define NUM_VALUES_MINE 3
int sensor_values[5][NUM_VALUES] = {0};
int sensor_iter[5] = {0};

int median_filter(int sensor, int new_value, int num_values) {
  int * values = sensor_values[sensor];
  int * iter = &sensor_iter[sensor];
  values[*iter] = new_value;
  *iter = (*iter + 1) % num_values;
  int values_cp[NUM_VALUES];
  for(int i = 0; i < num_values; i++)
    values_cp[i] = values[i];
  for(int i = num_values - 1; i > 0; i--) {
    for(int j = 0; j < i; j++) {
      if(values_cp[j] > values_cp[j + 1]) {
        int a = values_cp[j];
        values_cp[j] = values_cp[j + 1];
        values_cp[j + 1] = a;
      }
    }
  }
  return values_cp[num_values / 2];
}


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
  
  pinMode(HALL_NO_FLIP_PIN, INPUT);
  pinMode(HALL_FLIP_PIN, INPUT);
}

void hall_reset() {
  magnet_direction_flip = false, magnet_detection = false, magnet_side = false;
}

void get_hall_sensors() {
  hall_1 = analogRead(HALL_SENSOR_1_PIN);
  hall_1 = median_filter(0, hall_1, NUM_VALUES_HALL);
  hall_2 = analogRead(HALL_SENSOR_2_PIN);
  hall_2 = median_filter(1, hall_2, NUM_VALUES_HALL);
  hall_3 = analogRead(HALL_SENSOR_3_PIN);
  hall_3 = median_filter(2, hall_3, NUM_VALUES_HALL);
//  if(digitalRead(HALL_DETECTED_PIN) == HIGH) {
//    magnet_detection = true;
//  }
//  if(digitalRead(HALL_FLIP_PIN) == HIGH) {
//    magnet_direction_flip = true;
//  }
//  TODO: fix comparisons and tresholds - TEST
  if(max(max(hall_1, hall_2), hall_3) > 530 or
     min(min(hall_1, hall_2), hall_3) < 490) { // There is a magnet on top
    magnet_detection = true;
  }
  if(min(min(hall_1, hall_2), hall_3) < 490) { // There is a magnet on top
    magnet_direction_flip = true;
  }
  if(max(max(hall_1, hall_2), hall_3) > 560) { // There is a magnet on top
    magnet_direction_flip = false;
  }
  if(max(max(hall_1, hall_2), hall_3) > 560 or
     min(min(hall_1, hall_2), hall_3) < 460) { // There is a magnet on top
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
  L_mine_distance = median_filter(3, L_mine_distance, NUM_VALUES_MINE);
  R_mine_distance = IRsensor_R.distance();
  R_mine_distance = median_filter(4, R_mine_distance, NUM_VALUES_MINE);
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
