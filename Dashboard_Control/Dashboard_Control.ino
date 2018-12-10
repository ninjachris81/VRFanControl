#include <LogHelper.h>
#include <TaskManager.h>

#include "FanController.h"
#include "SmellController.h"
#include "CommController.h"
#include "LevelController.h"
#include "LedController.h"

#include "InitialBroadcastSupport.h"

TaskManager taskManager;

FanController fanController;
SmellController smellController;
CommController commController;
LevelController levelController;
LedController ledController;

void setup() {
  LOG_INIT();
  LOG_WAIT();

  taskManager.registerTask(&fanController);
  taskManager.registerTask(&smellController);
  taskManager.registerTask(&commController);
  taskManager.registerTask(&levelController);
  taskManager.registerTask(&ledController);
  
  taskManager.init();

  InitialBroadcastSupport::init();
  InitialBroadcastSupport::registerObject(&fanController);
  InitialBroadcastSupport::registerObject(&smellController);
  InitialBroadcastSupport::registerObject(&levelController);
  InitialBroadcastSupport::registerObject(&ledController);

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
