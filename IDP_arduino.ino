#include "config.h"
#include "drive.h"
#include "IR_sensors.h"

int state = STATE_STARTUP;


// function that runs first - before void loop()
void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  drive_init();
  sensors_init();
  drive_distance_verbose = false;
  drive_velocity_verbose = false;
}


void loop() {
//  drive_test();

  get_wall_position();
  get_mine_position();

  Serial.print(state);
  Serial.print("    L_mine ");
  Serial.print(L_mine_distance);
  Serial.print("\tR_mine ");
  Serial.print(R_mine_distance);
  Serial.print("\tF_wall ");
  Serial.print(F_wall_distance);
  Serial.print("\tL_wall ");
  Serial.print(L_wall_distance);
  Serial.print("\tR_wall ");
  Serial.print(R_wall_distance);
  Serial.print("\tmine_c ");
  Serial.print(mine_centered);
  Serial.print("\tmine_L ");
  Serial.print(mine_L);
  Serial.print("\tmine_R ");
  Serial.println(mine_R);

  if (state = STATE_STARTUP) {
    state = STATE_SEARCHING;
  }
  if (state = STATE_SEARCHING){
    
    if (mine_centered){
      drive_distance(10, 0);
      
      drive_velocity(1, 0);

      unsigned int timeout = 10000 * 30;
      do {
        get_hall_sensors();
        delayMicroseconds(100);
        timeout--;
      } while (!magnet_detection and timeout > 0);
      // TODO: pickup and flipping?
      drive_velocity(0, 0);
      state = STATE_CARRYING;
      
    } else if (mine_L){
      drive_distance(0.2, 1);
      
    } else if (mine_R){
      drive_distance(0.2, -1);
      
    } else if (F_wall_distance <= 25 and false){
      drive_distance(30, 0);
      drive_distance(-5, 90);
      drive_distance(35, 0);
      drive_distance(0, 90);
      drive_distance(-20, 0);
      drive_velocity(STANDARD_VEL, 0);
    } else {
      drive_velocity(STANDARD_VEL, 0);
    }
  } else if (state == STATE_CARRYING){
    state = STATE_SEARCHING;
    
  } else if (state == STATE_RETURNING){
    state = STATE_SEARCHING;
    
  }
}
