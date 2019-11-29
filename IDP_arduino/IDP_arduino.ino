#include "config.h"
#include "drive.h"
#include "IR_sensors.h"
#include <Servo.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 

int state = STATE_STARTUP, mine_counter = 0;

// function that runs first - before void loop()
void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  drive_init();
  sensors_init();
  drive_distance_verbose = false;
  drive_velocity_verbose = false;

  pickup(false, false);
}


void loop() {
  return;
//  drive_test();

  get_wall_position();
  get_mine_position();
  get_hall_sensors();

  Serial.print(state);
  Serial.print("\thall_1 ");
  Serial.print(hall_1);
  Serial.print("\thall_2 ");
  Serial.print(hall_2);
  Serial.print("\thall_3 ");
  Serial.print(hall_3);
//  Serial.print("\tL_mine ");
//  Serial.print(L_mine_distance);
//  Serial.print("\tR_mine ");
//  Serial.print(R_mine_distance);
//  Serial.print("\tF_wall ");
//  Serial.print(F_wall_distance);
//  Serial.print("\tL_wall ");
//  Serial.print(L_wall_distance);
//  Serial.print("\tR_wall ");
//  Serial.print(R_wall_distance);
//  Serial.print("\tmine_c ");
//  Serial.print(mine_centered);
//  Serial.print("\tmine_L ");
//  Serial.print(mine_L);
//  Serial.print("\tmine_R ");
//  Serial.print(mine_R);
  Serial.println();

  if (state == STATE_STARTUP) {
    drive_distance(25, 0);
    drive_distance(0, -90);
    state = STATE_SEARCHING;
  } else if (state == STATE_SEARCHING) {
    
    if (mine_centered){
      hall_reset();
      drive_distance(7, 0);
      
      drive_velocity(1.5, 0);

      long timeout = 1000 * 20;
      do {
        get_hall_sensors();
        delayMicroseconds(1000);
        if(magnet_detection) {
          timeout = min(timeout, 1400);
        }
        timeout--;
      } while (timeout > 0);
      drive_velocity(0, 0);
      
      pickup(magnet_side, magnet_direction_flip);
      
      drive_velocity(0, 0);
      state = STATE_CARRYING;
      
    } else if (mine_L){
      drive_distance(0.2, 1);
      
    } else if (mine_R){
      drive_distance(0.2, -1);
      
    } else if (F_wall_distance <= 25) {
      if (L_wall_distance <= 30 or R_wall_distance <= 30){
        state = STATE_RETURNING;
        return;
      }
      int move_by = 30;
      if (L_wall_distance <= 40 or R_wall_distance <= 40){
        move_by = 15;
      }
      int turn_angle = 0 - angle;
      drive_distance(30, 0);
      drive_distance(-10, 0);
      drive_distance(0, turn_angle);
      drive_distance(move_by, 0);
      drive_distance(0, turn_angle);
      drive_distance(-30, 0);
      drive_velocity(STANDARD_VEL, 0);
    } else {
      drive_velocity(STANDARD_VEL, 0);
    }
  } else if (state == STATE_CARRYING){
    drive_distance(0, 180 - angle);
    drive_velocity(STANDARD_VEL, 0);
    float time_drive = millis();
    do {
      get_wall_position();
      delayMicroseconds(1000);
    } while (F_wall_distance > 25);
    time_drive = millis() - time_drive;
    drive_distance(30, 0);
    drive_distance(-5, 0);
    drive_distance(0, 90);
    drive_velocity(STANDARD_VEL, 0);
    do {
      get_wall_position();
      delayMicroseconds(1000);
    } while (F_wall_distance > 25);
    
    drop_off(magnet_side, magnet_direction_flip);

    drive_distance(0, 90);
    drive_distance(-30, 0);
    float distance = time_drive * STANDARD_VEL / VEL2DIS_TIME_MULT;
    drive_distance(max(distance - 10, 40.0), 0);
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
    } while (F_wall_distance > 25);
    drive_distance(30, 0);
    drive_distance(-5, 0);
    drive_distance(0, -90);
    drive_velocity(STANDARD_VEL, 0);
    do {
      get_wall_position();
      delayMicroseconds(1000);
    } while (F_wall_distance > 15);
    drive_velocity(0, 0);
    state = STATE_FINISHED;
  }
}



//for WIFI
//void setup() {
//  // initialize serial and wait for the port to open:
//  Serial.begin(9600);
//  while(!Serial) ;
//
//  // attempt to connect using WEP encryption:
//  Serial.println("Initializing Wifi...");
//  printMacAddress();
//
//  // scan for existing networks:
//  Serial.println("Scanning available networks...");
//  listNetworks();
//}
//
//void loop() {
//  delay(10000);
//  // scan for existing networks:
//  Serial.println("Scanning available networks...");
//  listNetworks();
//}
//
//void printMacAddress() {
//  // the MAC address of your Wifi shield
//  byte mac[6];                     
//
//  // print your MAC address:
//  WiFi.macAddress(mac);
//  Serial.print("MAC: ");
//  Serial.print(mac[5],HEX);
//  Serial.print(":");
//  Serial.print(mac[4],HEX);
//  Serial.print(":");
//  Serial.print(mac[3],HEX);
//  Serial.print(":");
//  Serial.print(mac[2],HEX);
//  Serial.print(":");
//  Serial.print(mac[1],HEX);
//  Serial.print(":");
//  Serial.println(mac[0],HEX);
//}
//
//void listNetworks() {
//  // scan for nearby networks:
//  Serial.println("** Scan Networks **");
//  byte numSsid = WiFi.scanNetworks();
//
//  // print the list of networks seen:
//  Serial.print("number of available networks:");
//  Serial.println(numSsid);
//
//  // print the network number and name for each network found:
//  for (int thisNet = 0; thisNet<numSsid; thisNet++) {
//    Serial.print(thisNet);
//    Serial.print(") ");
//    Serial.print(WiFi.SSID(thisNet));
//    Serial.print("\tSignal: ");
//    Serial.print(WiFi.RSSI(thisNet));
//    Serial.print(" dBm");
//    Serial.print("\tEncryption: ");
//    Serial.println(WiFi.encryptionType(thisNet));
//  }
//}
