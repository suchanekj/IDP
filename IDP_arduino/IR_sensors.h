#include "config.h"

extern int F_wall_distance, L_wall_distance, R_wall_distance, L_mine_distance, R_mine_distance, hall_1, hall_2, hall_3;
extern bool mine_centered, mine_L, mine_R;
extern bool magnet_direction_flip, magnet_detection, magnet_side;

void sensors_init();
void hall_reset();
void get_wall_position();
void get_mine_position();
void get_hall_sensors();
