#include "SmellController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "FanController.h"
#include "ServoController.h"

SmellController::SmellController() : AbstractTriggerTask() {
}

void SmellController::init() {
}

void SmellController::update() {
  switch(smellPhase) {
    case SMELL_OFF:
      break;
    case SMELL_INIT_FAN:
      LOG_PRINT(F("Trigger servo "));
      LOG_PRINTLN(currentEmitCount);
      
      currentEmitCount--;
      if (currentEmitCount==0) smellPhase = SMELL_VAPO;
      
      taskManager->getTask<ServoController*>(SERVO_CONTROLLER)->triggerAction(currentLocation-1);

      triggerUpdateDelay(1000);     // duration of triggeraction
      break;
    case SMELL_VAPO:
      LOG_PRINTLN(F("Fan delay"));
      smellPhase = SMELL_DELAY_FAN;
      triggerUpdateDelay(1000);     // delay the fan for 1 sec
      break;
    case SMELL_DELAY_FAN:
      LOG_PRINTLN(F("Smell finished"));
      smellPhase = SMELL_OFF;
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->restoreCurrentSpeeds();
      break;
  }
}

void SmellController::releaseSmell(SmellController::SMELL_LOCATION location, uint8_t count) {
  if (count>10) return;
  if (smellPhase!=SMELL_OFF) {
    LOG_PRINTLN(F("Smell ongoing"));
    return;
  }
  
  currentEmitCount = count;
  currentLocation = location;
  
  LOG_PRINT(F("Setting smell at "));
  LOG_PRINT(location);
  LOG_PRINT(F(", count "));
  LOG_PRINTLN(count);

  smellPhase = SMELL_INIT_FAN;
  LOG_PRINTLN(F("Init fan"));

  taskManager->getTask<FanController*>(FAN_CONTROLLER)->saveCurrentSpeeds();
  
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(FanController::SPEED_LOCATION_LEFT, EMIT_SPEED_LEVEL);
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(FanController::SPEED_LOCATION_RIGHT, EMIT_SPEED_LEVEL);
  
  triggerUpdateDelay(500);    // give the fan 500 ms to turn on
}
