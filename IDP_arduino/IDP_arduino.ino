#include "config.h"
#include "drive.h"
#include "IR_sensors.h"
#include "wifi.h"
#include <Servo.h>
//#include <SPI.h>
//#include <WiFiNINA.h>

int state = STATE_STARTUP, mine_counter = 0;
long time_led = 0, mine_time = 0;
long mine_timeout = 0;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  drive_init();
  sensors_init();
//  digitalWrite(AMBER_LED, HIGH);
//  digitalWrite(RED_LED, HIGH);
//  delay(100000);
  
  drive_distance_verbose = false;
  drive_velocity_verbose = false;
  delay(1000);
  wifi_init();
  delay(1000);
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
//  return;

  wifi_run();
//  return;
  
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

  if(halted and millis() - last_command_time < 5000) {
    Serial.println(" HALTED");
    return;
  }
  
  Serial.println();
//  return;

  if(state != STATE_SEARCHING) {
    mine_timeout = millis();
  }

  if (state == STATE_STARTUP) {
    drive_distance(20, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    drive_distance(0, -87);
    state = STATE_SEARCHING;
  } else if (state == STATE_SEARCHING) {
    
    if (mine_centered or L_mine_distance < 8 or R_mine_distance < 8 and F_wall_distance > 20){
      Serial.println("mine centered");
      hall_reset();
      if(mine_centered) // V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V V 
        drive_distance(0, -3 + ( - R_mine_distance + L_mine_distance) / 2);
      else if(L_mine_distance < 10) {
        drive_velocity(0, 0);
//        delay(5000);
        drive_distance(0, -13);
      } else if(R_mine_distance < 10) {
        drive_velocity(0, 0);
//        delay(10000);
        drive_distance(0, 13);
      } // /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ /\ 
      drive_distance(5, 0);
      

      digitalWrite(AMBER_LED, HIGH);

      for(int i = 0; i < 3; i++) {
        drive_velocity(2.0, 0);
        long timeout = 1000 * 5;
        do {
          get_hall_sensors();
          delayMicroseconds(1000);
//          Serial.print(magnet_detection);
//          Serial.print("\t");
//          Serial.print(magnet_side);
//          Serial.print("\t");
//          Serial.print(magnet_direction_flip);
//          Serial.println();
          if(magnet_detection) {
            timeout = min(timeout, 1000);
          }
          timeout--;
        } while (timeout > 0);
        if(magnet_detection) break;
        if(i == 0) {
          drive_distance(-20, 0);
          drive_distance(0, -6);
        }
        if(i == 1) {
          drive_distance(-20, 0);
          drive_distance(0, 12);
        }
        if(i == 2) {
          drive_distance(-30, 0);
          return;
        }
      }
      
      digitalWrite(AMBER_LED, LOW);
      mine_in = true;
      
      Serial.println("hall detected");
      drive_velocity(0, 0);
      
      pickup(magnet_side, magnet_direction_flip);

      Serial.println("picked up");
      
      drive_velocity(0, 0);
      state = STATE_CARRYING;
      
    } else if (mine_L){
      if(millis() - mine_timeout > 5000) {
        drive_distance(-15, 0);
        drive_distance(0, -15);
        mine_timeout = millis();
        return;
      }
      drive_distance(0.2, 1);
      mine_time = millis();
      
    } else if (mine_R){
      if(millis() - mine_timeout > 5000) {
        drive_distance(-15, 0);
        drive_distance(0, 15);
        mine_timeout = millis();
        return;
      }
      drive_distance(0.2, -1);
      mine_time = millis();
    } else {
      mine_timeout = millis();
      if(millis() - last_command_time < 10000 and millis() - mine_time > 1000 and mine_counter != 0) {
        if(digitalRead(FRONT_L_PIN) == HIGH and digitalRead(FRONT_R_PIN) == HIGH) {
          drive_distance(-30, 0);
          return;
        }
        if(command_angle != 0) {
          drive_distance(0, command_angle);
          wifi_clear();
          command_angle = 0;
        }
      } else if (F_wall_distance <= 30 and millis() - last_command_time > 10000) {
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
        if(angle < 0) angle = -90;
        else angle = 90;
        drive_velocity(STANDARD_VEL, 0);
      } else if(millis() - mine_time > 1000) {
        if(angle > 0) drive_distance(0, 90 - angle);
        else drive_distance(0, -90 - angle);
      }
      drive_velocity(SEARCHING_VEL, 0);
    }
  } else if (state == STATE_CARRYING){
    if(angle > 0)
      drive_distance(0, 180 - angle);
    else
      drive_distance(0, -180 - angle);
    drive_distance(0, -10);
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
    drive_distance(55, 0);
    drive_distance(-3, 0);
    drive_distance(0, 84);
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

    drive_distance(-5, 0);
    drive_distance(0, 90);
    drive_distance(-30, 0);
    angle = 0;
    float distance = time_drive * STANDARD_VEL / VEL2DIS_TIME_MULT;
    float distance_drive = max(distance - 40, 0);
    drive_distance(40, 0);
    wifi_run();
    if(millis() - last_command_time < 10000) {
      wifi_clear();
      command_angle = 0;
      drive_velocity(SEARCHING_VEL, 0);
    } else {
      drive_distance(distance_drive, 0);
      drive_distance(0, 90);
    }
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
