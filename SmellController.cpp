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
    delay(1000);
    digitalWrite(PIN_SMELL, LOW);
    delay(1000);
  }*/
}

void SmellController::update() {
  switch(smellPhase) {
    case SMELL_OFF:
      break;
    case SMELL_INIT_FAN:
      smellPhase = SMELL_VAPO;
      
      LOG_PRINT(F("Smell on"));
      digitalWrite(PIN_SMELL, LOW);   // smell on
      triggerUpdateDelay(currentIntensity);
      break;
    case SMELL_VAPO:
      smellPhase = SMELL_DELAY_FAN;
      LOG_PRINT(F("Smell off"));
      digitalWrite(PIN_SMELL, HIGH);  // smell off
      triggerUpdateDelay(1000);     // delay the fan for 1 sec
      break;
    case SMELL_DELAY_FAN:
      smellPhase = SMELL_OFF;
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(SPEED_RIGHT, orgSpeedLevel);
      break;
  }
}

void SmellController::releaseSmell(uint16_t intensity) {
  if (intensity<=0 || intensity>9999) return;
  if (smellPhase!=SMELL_OFF) {
    LOG_PRINTLN(F("Smell ongoing"));
    return;
  }
  
  currentIntensity = intensity;
  
  LOG_PRINT(F("Setting smell for "));
  LOG_PRINTLN(currentIntensity);

  smellPhase = SMELL_INIT_FAN;
  orgSpeedLevel = taskManager->getTask<FanController*>(FAN_CONTROLLER)->getSpeedLevel(SPEED_RIGHT);
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(SPEED_RIGHT, 5);
  triggerUpdateDelay(500);    // give the fan 500 ms to turn on
}
