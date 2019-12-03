#include "config.h"
#include "drive.h"
#include "IR_sensors.h"
#include "wifi.h"
#include <Servo.h>
//#include <SPI.h>
//#include <WiFiNINA.h>

int state = STATE_STARTUP, mine_counter = 0;
long time_led = 0;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
//  drive_init();
//  sensors_init();
  
  drive_distance_verbose = false;
  drive_velocity_verbose = false;
//  delay(5000);
  wifi_init();
}


void loop() {
//  magnet_side = true;
//  magnet_direction_flip = true;
//  pickup(magnet_side, magnet_direction_flip);
//  delay(3000);
//  drop_off(magnet_side, magnet_direction_flip);
//  delay(200000);
//  picker_test();
//  drive_test();
  wifi_run();
  return;

  hall_reset();
  get_wall_position();
  get_mine_position();
  get_hall_sensors();

  Serial.print(state);
  Serial.print("\t");
  Serial.print(magnet_detection);
  Serial.print("\t");
  Serial.print(magnet_side);
  Serial.print("\t");
  Serial.print(magnet_direction_flip);
//        Serial.println();
  Serial.print("\thall_1 ");
  Serial.print(hall_1);
  Serial.print("\thall_2 ");
  Serial.print(hall_2);
  Serial.print("\thall_3 ");
  Serial.print(hall_3);
  Serial.print("\tL_mine ");
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
  Serial.print(mine_R);
  Serial.println();
//  return;

  if (state == STATE_STARTUP) {
    drive_distance(20, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    drive_distance(0, -85);
    state = STATE_SEARCHING;
  } else if (state == STATE_SEARCHING) {
    
    if (mine_centered or L_mine_distance < 8 or R_mine_distance < 8){
      Serial.println("mine centered");
      hall_reset();
      if(mine_centered)
        drive_distance(0, -3);
      else if(L_mine_distance < 8)
        drive_distance(0, -6);
      else if(R_mine_distance < 8)
        drive_distance(0, 6);
      drive_distance(5, 0);
      
      drive_velocity(2.0, 0);

      digitalWrite(AMBER_LED, HIGH);

      long timeout = 1000 * 20;
      do {
        get_hall_sensors();
        delayMicroseconds(1000);
        Serial.print(magnet_detection);
        Serial.print("\t");
        Serial.print(magnet_side);
        Serial.print("\t");
        Serial.print(magnet_direction_flip);
        Serial.println();
        if(magnet_detection) {
          timeout = min(timeout, 1000);
        }
        timeout--;
      } while (timeout > 0);
     
      digitalWrite(AMBER_LED, LOW);
      mine_in = true;
      
      Serial.println("hall detected");
      drive_velocity(0, 0);
      
//      pickup(magnet_side, magnet_direction_flip);  *************************************************

      Serial.println("picked up");
      
      drive_velocity(0, 0);
      state = STATE_CARRYING;
      
    } else if (mine_L){
      drive_distance(0.2, 1);
      
    } else if (mine_R){
      drive_distance(0.2, -1);
      
    } else if (F_wall_distance <= 30) {
//      if (L_wall_distance <= 30 or R_wall_distance <= 30){
//        state = STATE_RETURNING;
//        return;
//      }
      int move_by = 40;
//      if (L_wall_distance <= 40 or R_wall_distance <= 40){
//        move_by = 15;
//      }
      int turn_angle;
      if (angle < 0)
        turn_angle = 90;
      else
        turn_angle = -90;
      drive_distance(40, 0);
      drive_distance(-5, 0);
      drive_distance(0, turn_angle);
      drive_distance(move_by, 0);
      drive_distance(0, turn_angle);
      drive_distance(-40, 0);
      drive_velocity(STANDARD_VEL, 0);
    } else {
      drive_velocity(STANDARD_VEL, 0);
    }
  } else if (state == STATE_CARRYING){
    if(angle > 0)
      drive_distance(0, 180 - angle);
    else
      drive_distance(0, -180 - angle);
    drive_velocity(STANDARD_VEL, 0);
    long time_drive = millis();
    do {
      get_wall_position();
      delayMicroseconds(1000);
      if(millis() - time_led >= 250) {
        digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
        digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);
        time_led = millis();
      }
    } while (F_wall_distance > 25);
    time_drive = millis() - time_drive;
    drive_distance(40, 0);
    drive_distance(-5, 0);
    drive_distance(0, 87);
    drive_velocity(STANDARD_VEL, 0);
    do {
      get_wall_position();
      delayMicroseconds(1000);
      if(millis() - time_led >= 250) {
        digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
        digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);
        time_led = millis();
      }
    } while (F_wall_distance > 25);
    
    drop_off(magnet_side, magnet_direction_flip);
    mine_in = false;

    drive_distance(0, 90);
    drive_distance(-30, 0);
    float distance = time_drive * STANDARD_VEL / VEL2DIS_TIME_MULT;
    drive_distance(max(distance + 10, 45.0), 0);
    drive_distance(0, 90);
    mine_counter += 1;
    if (mine_counter != 8){
      state = STATE_SEARCHING;
    } else {
      state = STATE_RETURNING;
    }
  } else if (state == STATE_RETURNING){
    drive_distance(0, 180 - angle);
    drive_velocity(STANDARD_VEL, 0);
    do {
      get_wall_position();
      delayMicroseconds(1000);
      if(millis() - time_led >= 250) {
        digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
        time_led = millis();
      }
    } while (F_wall_distance > 25);
    drive_distance(30, 0);
    drive_distance(-5, 0);
    drive_distance(0, -90);
    drive_velocity(STANDARD_VEL, 0);
    do {
      get_wall_position();
      delayMicroseconds(1000);
      if(millis() - time_led >= 250) {
        digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
        time_led = millis();
      }
    } while (F_wall_distance > 15);
    drive_velocity(0, 0);
    state = STATE_FINISHED;
  }
  
  if(millis() - time_led >= 250) {
    digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
    time_led = millis();
  }
}
