#include <LogHelper.h>
#include <TaskManager.h>

#include "SeatController.h"
#include "CommController.h"

TaskManager taskManager;

SeatController seatController;
CommController commController;

void setup() {
  LOG_INIT();
  //LOG_WAIT();

  taskManager.registerTask(&seatController);  
  taskManager.registerTask(&commController);
  
  taskManager.init();

  LOG_PRINTLN(F("Init complete"));
}


void loop() {
  taskManager.update();
  delay(10);
}
