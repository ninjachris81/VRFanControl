#include <LogHelper.h>
#include <TaskManager.h>

// either wifi or serial
#define USE_WIFI

#include "FanController.h"
#include "SmellController.h"
#include "ServoController.h"

#ifdef USE_WIFI
  #include "WifiController.h"
#else
  #include "CommController.h"
#endif

TaskManager taskManager;

FanController fanController;
SmellController smellController;
ServoController servoController;

#ifdef USE_WIFI
  WifiController wifiController;
#else
  CommController commController;
#endif

void setup() {
  LOG_INIT();
  LOG_WAIT();
  
  taskManager.registerTask(&fanController);
  taskManager.registerTask(&smellController);
  taskManager.registerTask(&servoController);
#ifdef USE_WIFI
  taskManager.registerTask(&wifiController);
#else
  taskManager.registerTask(&commController);
#endif
  taskManager.init();

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
