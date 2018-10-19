#include <LogHelper.h>
#include <TaskManager.h>

#include "FanController.h"
#include "SmellController.h"
#include "CommController.h"
#include "LevelController.h"

TaskManager taskManager;

FanController fanController;
SmellController smellController;
CommController commController;
LevelController levelController;

void setup() {
  LOG_INIT();
  LOG_WAIT();

  taskManager.registerTask(&fanController);
  taskManager.registerTask(&smellController);
  taskManager.registerTask(&commController);
  taskManager.registerTask(&levelController);
  
  taskManager.init();

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
