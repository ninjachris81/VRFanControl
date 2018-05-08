/* META INFOS
target_package=arduino
target_platform=avr
board=nano
*/
#include <LogHelper.h>
#include <TaskManager.h>

#include "FanController.h"
#include "CommController.h"
#include "SmellController.h"

TaskManager taskManager;

FanController fanController;
CommController commController;
SmellController smellController;

void setup() {
  LOG_INIT();
  LOG_WAIT();
  
  taskManager.registerTask(&fanController);
  taskManager.registerTask(&commController);
  taskManager.registerTask(&smellController);
  taskManager.init();

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
