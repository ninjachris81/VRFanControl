#include "SmellController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "FanController.h"

SmellController::SmellController() : AbstractTriggerTask() {
}

void SmellController::init() {
  pinMode(PIN_SMELL, OUTPUT);
  digitalWrite(PIN_SMELL, HIGH);

  // turn on
  /*
  for (uint8_t i=0;i<5;i++) {
    digitalWrite(PIN_SMELL, HIGH);
    delay(200);
    digitalWrite(PIN_SMELL, LOW);
    delay(200);
  }*/
}

void SmellController::update() {
  switch(smellPhase) {
    case SMELL_OFF:
      break;
    case SMELL_INIT_FAN:
      smellPhase = SMELL_VAPO;
      
      digitalWrite(PIN_SMELL, LOW);   // smell on
      //triggerUpdateDelay(currentIntensity);
      break;
    case SMELL_VAPO:
      smellPhase = SMELL_DELAY_FAN;
      digitalWrite(PIN_SMELL, HIGH);  // smell off
      triggerUpdateDelay(1000);     // delay the fan for 1 sec
      break;
    case SMELL_DELAY_FAN:
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
  
  LOG_PRINT(F("Setting smell at"));
  LOG_PRINT(location);
  LOG_PRINT(F(", count "));
  LOG_PRINTLN(count);

  smellPhase = SMELL_INIT_FAN;

  taskManager->getTask<FanController*>(FAN_CONTROLLER)->saveCurrentSpeeds();
  
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(FanController::SPEED_LOCATION_RIGHT, EMIT_SPEED_LEVEL);
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(FanController::SPEED_LOCATION_LEFT, EMIT_SPEED_LEVEL);
  triggerUpdateDelay(500);    // give the fan 500 ms to turn on
}
