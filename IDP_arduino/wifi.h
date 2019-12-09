extern int command_angle;
extern long last_command_time;
extern bool halted;

void wifi_init();
void wifi_run();
//char listen_wifi();
void respond_command (byte I);
void printWifiStatus();
void wifi_clear();
