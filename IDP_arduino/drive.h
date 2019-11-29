#include "config.h"

extern bool drive_distance_verbose;
extern bool drive_velocity_verbose;
extern bool robotIsMoving;
extern float angle;

void pickup(bool top, bool flip);
void drop_off(bool top, bool flip);
void drive_distance(float distance_cm, float angular_distance_deg);
void drive_velocity(float speed_cms, float angular_speed_degs);
void drive_init();
void drive_test();
