#define FAN_CONTROLLER 0
#define SMELL_CONTROLLER 1
#define SERVO_CONTROLLER 2
#define SEAT_CONTROLLER 3

#ifdef USE_WIFI
  #define WIFI_CONTROLLER 4
#else
  #define COMM_CONTROLLER 4
#endif
