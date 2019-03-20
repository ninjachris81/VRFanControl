#include <LogHelper.h>
#include <TaskManager.h>

//#define LEVEL_SUPPORT

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
LedController ledController;
#ifdef LEVEL_SUPPORT
  LevelController levelController;
#endif

void setup() {
  LOG_INIT();
  LOG_WAIT();

  taskManager.registerTask(&fanController);
  taskManager.registerTask(&smellController);
  taskManager.registerTask(&commController);
  taskManager.registerTask(&ledController);
#ifdef LEVEL_SUPPORT
  taskManager.registerTask(&levelController);
#endif
  
  taskManager.init();

  InitialBroadcastSupport::init();
  InitialBroadcastSupport::registerObject(&fanController);
  InitialBroadcastSupport::registerObject(&smellController);
  InitialBroadcastSupport::registerObject(&ledController);
#ifdef LEVEL_SUPPORT
  InitialBroadcastSupport::registerObject(&levelController);
#else
  LOG_PRINTLN(F("No Level support"));
#endif

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
