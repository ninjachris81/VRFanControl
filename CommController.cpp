#include "CommController.h"

#include <LogHelper.h>

#include "TaskIDs.h"
#include "FanController.h"
#include "SmellController.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  
}

void CommController::update() {
  if (Serial.available()>0) {
    String s = Serial.readStringUntil('\n');
    uint8_t param1 = s.substring(2).toInt();

    LOG_PRINTLN(s);
    LOG_PRINTLN(param1);

    if (s.startsWith("sl")) {
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(SPEED_LEFT, param1);
    } else if (s.startsWith("sr")) {
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(SPEED_RIGHT, param1);
    } else if (s.startsWith("sm")) {
      taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->releaseSmell(param1);
    } else {
      LOG_PRINT(F("Unknown command: "));
      LOG_PRINTLN(s);
    }
  }
}

