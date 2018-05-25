#include "CommController.h"

#include <LogHelper.h>

#include "TaskIDs.h"
#include "FanController.h"
#include "SmellController.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  LOG_PRINTLN(F("Available commands:"));
  LOG_PRINTLN(F("sl[0-9] -> speed left"));
  LOG_PRINTLN(F("sr[0-9] -> speed right"));
  LOG_PRINTLN(F("sm[1-9999] -> smell"));
}

void CommController::update() {
  if (Serial.available()>0) {
    String s = Serial.readStringUntil('\n');
    int param1 = s.substring(2).toInt();

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

