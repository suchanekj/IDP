#include <Arduino.h>
#include <Servo.h>
#include "drive.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

bool drive_distance_verbose = false;
bool drive_velocity_verbose = false;
bool robotIsMoving = false;
float angle = 0;

long time_drive_velocity = -1;
float ang_speed_drive_velocity = 0;

bool moving = false, mine_in = false;
//int timer2_counter;

//void interrupts_init() {
//  noInterrupts();           // disable all interrupts
//
//  TCB2.CCMP = 200000;
//  TCB2.INTCTRL =  (1 << TCB_CAPT_bp);   // Enable interrupts
//  TCB2.INTCTRL |= (1 << TCB_CAPT_bp);   // Enable interrupts
//  TCB2.CTRLA |= TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm;  // Enable timer with prescaler of /2 to start with 10MHz frequency.
//  interrupts();             // enable all interrupts
//}
//
//ISR(TCB3_INT_vect)
//{
//  //set flags
//  TCB3.INTFLAGS = TCB_CAPT_bm;
//  if(moving)
//    digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
//  else
//    digitalWrite(AMBER_LED, 0);
//  if(mine_in)
//    digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);
//  else
//    digitalWrite(RED_LED, 0);
//}

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *MotorL = AFMS.getMotor(3);
Adafruit_DCMotor *MotorR = AFMS.getMotor(4);
Adafruit_DCMotor *MotorTop = AFMS.getMotor(1);
Adafruit_DCMotor *MotorBottom = AFMS.getMotor(2);
Servo servoPicker;

#define SERVO_POSITION_BOTTOM 96
#define SERVO_POSITION_TOP 119
#define SERVO_POSITION_LEVEL 130

//start the motor
void drive_init() {
  AFMS.begin();
  servoPicker.attach(10);
  servoPicker.write(SERVO_POSITION_LEVEL);
  MotorTop->run(BACKWARD);
  MotorTop->setSpeed(120);
  MotorBottom->run(BACKWARD);
  MotorBottom->setSpeed(200);
  delay(2000);
  MotorTop->setSpeed(0);
  MotorBottom->setSpeed(0);

  
  pinMode(AMBER_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(AMBER_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

void picker_test() {
  servoPicker.write(SERVO_POSITION_BOTTOM);
  delay(2000);
  servoPicker.write(SERVO_POSITION_TOP);
  delay(2000);
  servoPicker.write(SERVO_POSITION_LEVEL);
  delay(2000);
  MotorTop->run(BACKWARD);
  MotorTop->setSpeed(100);
  delay(2000);
  MotorTop->run(FORWARD);
  MotorTop->setSpeed(100);
  delay(2000);
  MotorBottom->run(BACKWARD);
  MotorBottom->setSpeed(100);
  delay(2000);
  MotorBottom->run(FORWARD);
  MotorBottom->setSpeed(100);
  delay(2000);
}

void pickup(bool top, bool flip) {
  Serial.println("position picker");
  MotorTop->run(BACKWARD);
  MotorTop->setSpeed(120);
  MotorBottom->run(BACKWARD);
  MotorBottom->setSpeed(200);
  delay(2000);
  if(!top) {
    servoPicker.write(SERVO_POSITION_BOTTOM);
  } else {
    servoPicker.write(SERVO_POSITION_TOP);
  }
  delay(500);
  MotorTop->setSpeed(0);
  MotorBottom->setSpeed(0);
  drive_velocity(3, 0);
  
  delay(3000);
  Serial.println("level picker");
  servoPicker.write(SERVO_POSITION_LEVEL);
  drive_velocity(0, 0);
  delay(300);

  if(flip) {
    Serial.println("flip");
    if(top) {
      MotorBottom->run(FORWARD);
      MotorBottom->setSpeed(200);
      MotorTop->run(FORWARD);
      MotorTop->setSpeed(100);
      delay(1000);
      MotorTop->setSpeed(0);
      delay(2000);
      MotorBottom->setSpeed(0);
    } else {
      MotorTop->run(FORWARD);
      MotorTop->setSpeed(160);
      delay(2000);
      MotorTop->run(BACKWARD);
      delay(2000);
      MotorTop->setSpeed(0);
    }
  }
}

void drop_off(bool top, bool flip) {
  drive_distance(10, 0);
  if(flip) {
    top = !top;
  }
  drive_distance(20, 0);
  
//  if(top) {
//    servoPicker.write(SERVO_POSITION_TOP);
//  } else {
//    servoPicker.write(SERVO_POSITION_BOTTOM);
//  }
  MotorTop->run(FORWARD);
  MotorTop->setSpeed(150);
  MotorBottom->run(FORWARD);
  MotorBottom->setSpeed(230);
  delay(2500);

  servoPicker.write(SERVO_POSITION_LEVEL);
  MotorTop->run(BACKWARD);
  MotorTop->setSpeed(120);
  MotorBottom->run(BACKWARD);
  MotorBottom->setSpeed(200);
  delay(2000);
  drive_distance(-20, 0);
  MotorTop->setSpeed(0);
  MotorBottom->setSpeed(0);
}

//testing drive
void drive_test(){

//  Serial.println("left motor activated");
//  MotorL->run(FORWARD);
//  MotorL->setSpeed(100);
//  delay (3000);
//  MotorL->setSpeed(0);
//
//  Serial.println("right motor activated");
//  MotorR->run(FORWARD);
//  MotorR->setSpeed(100);
//  delay(3000);
//  MotorR->setSpeed(0);

  delay(5000);
  
//  Serial.println("move along a straight line of 50cm");
//  drive_distance(50, 0);
//
//  delay(5000);

  Serial.println("turn right 180deg");
  drive_distance(0, 180);

  delay(5000);

  Serial.println("turn left 180deg");
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);
  drive_distance(0, -30);
  drive_velocity(0, 0);
  delay(100);

  delay(15000);
}

// Distance the robot will move
void drive_distance(float distance_cm, float angular_distance_deg) {
  
  if(time_drive_velocity >= 0) {
    angle += ang_speed_drive_velocity * (millis() - time_drive_velocity) / 1000;
    while (angle <= -180) angle += 360;
    while (angle > 180) angle -= 360;
    time_drive_velocity = -1;
  }
  
  float distance = distance_cm, angular_distance = -angular_distance_deg * PI / 180;
  float distanceL, distanceR;
  
  if(fabs(angular_distance) > 1e-6) {
    float radius = distance / angular_distance;
    float radiusL = radius + WHEEL_SEPARATION / 2.0;
    float radiusR = radius - WHEEL_SEPARATION / 2.0;
    distanceL = radiusL * angular_distance;
    distanceR = radiusR * angular_distance;
        
  } else {
    distanceL = distance;
    distanceR = distance;
  }
  
  float max_dis = max(fabs(distanceL), fabs(distanceR));
  float velocityL = distanceL * STANDARD_VEL / max_dis * CMS2POWER, velocityR = distanceR * STANDARD_VEL / max_dis * CMS2POWER;
  float time_wait = max_dis / STANDARD_VEL * VEL2DIS_TIME_MULT;

  MotorL->setSpeed(abs(velocityL) * POWER_L_TO_R);
  if (velocityL < 0){
    MotorL->run(DRIVE_BACKWARD);
  } else {
    MotorL->run(DRIVE_FORWARD);
  }

  MotorR->setSpeed(abs(velocityR) / POWER_L_TO_R);
  if (velocityR < 0){
    MotorR->run(DRIVE_BACKWARD);
  } else {
    MotorR->run(DRIVE_FORWARD);
  }

  if(drive_distance_verbose) {
    Serial.print("Driving for ");
    Serial.print(time_wait);
    Serial.print("ms with speed_L ");
    Serial.print(velocityL);
    Serial.print(" and speed_R ");
    Serial.print(velocityR);
    Serial.println();
  }

  if(time_wait > 100) {
    digitalWrite(AMBER_LED, HIGH);
    if(mine_in) digitalWrite(RED_LED, HIGH);
    else digitalWrite(RED_LED, LOW);
    for(int i = 0; i < int(time_wait / 250); i++) {
      digitalWrite(AMBER_LED, digitalRead(AMBER_LED) ^ 1);
      if(mine_in) digitalWrite(RED_LED, digitalRead(RED_LED) ^ 1);
      delay(250);
    }
    delay(int(time_wait) % 250);
    digitalWrite(AMBER_LED, LOW);
    digitalWrite(RED_LED, LOW);
  }
  else delay(time_wait);
  
  drive_velocity(0, 0);
  delayMicroseconds(1000);
  
  angle += angular_distance_deg;
  while (angle <= -180) angle += 360;
  while (angle > 180) angle -= 360;
}


void drive_velocity(float speed_cms, float angular_speed_degs) {
  if(time_drive_velocity >= 0) {
    angle += ang_speed_drive_velocity * (millis() - time_drive_velocity) / 1000;
    while (angle <= -180) angle += 360;
    while (angle > 180) angle -= 360;
    time_drive_velocity = -1;
  }
  if(fabs(speed_cms) + fabs(angular_speed_degs) < 1e-6) {
     digitalWrite(AMBER_LED, LOW);
  }
  
  float speed = speed_cms, angular_speed = -angular_speed_degs * PI / 180;
  float velocityL, velocityR;
  
  if(fabs(angular_speed) > 1e-6) {
    float radius = speed / angular_speed;
    float radiusL = radius + WHEEL_SEPARATION / 2;
    float radiusR = radius - WHEEL_SEPARATION / 2;
    
    velocityL = radiusL * angular_speed * CMS2POWER;
    velocityR = radiusR * angular_speed * CMS2POWER;
    
  } else {
    velocityL = speed_cms * CMS2POWER;
    velocityR = speed_cms * CMS2POWER;
  }

  MotorL->setSpeed(abs(velocityL) * POWER_L_TO_R);
  if (velocityL < 0){
    MotorL->run(DRIVE_BACKWARD);
  } else {
    MotorL->run(DRIVE_FORWARD);
  }

  MotorR->setSpeed(abs(velocityR) / POWER_L_TO_R);
  if (velocityR < 0){
    MotorR->run(DRIVE_BACKWARD);
  } else {
    MotorR->run(DRIVE_FORWARD);
  }
  
  if(drive_velocity_verbose) {
    Serial.print("Driving with speed_L ");
    Serial.print(velocityL);
    Serial.print(" and speed_R ");
    Serial.print(velocityR);
    Serial.println();
  }
  time_drive_velocity = millis();
  ang_speed_drive_velocity = angular_speed_degs;
}
