#include "SmellController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "FanController.h"

SmellController::SmellController() : AbstractTriggerTask() {
}

void SmellController::init() {
  pinMode(PIN_MM_RELAY1, OUTPUT);
  pinMode(PIN_MM_RELAY2, OUTPUT);
  pinMode(PIN_MM_RELAY3, OUTPUT);

  digitalWrite(PIN_MM_RELAY1, HIGH);
  digitalWrite(PIN_MM_RELAY2, HIGH);
  digitalWrite(PIN_MM_RELAY3, HIGH);
  
  for (uint8_t i=0;i<MM_COUNT;i++) {
    mmStates[i].init(i, 0);
    mmStates[i].registerValueChangeListener(this);
  }
}

void SmellController::update() {
  switch(smellPhase) {
    case SMELL_OFF:
      break;
    case SMELL_INIT_FAN:
      smellPhase = SMELL_VAPO;
      mmStates[currentLocation].setValue(true);
      triggerUpdateDelay(currentIntensity);
      break;
    case SMELL_VAPO:
      LOG_PRINTLN(F("Fan delay"));
      smellPhase = SMELL_DELAY_FAN;
      mmStates[currentLocation].setValue(false);
      triggerUpdateDelay(1000);     // delay the fan for 1 sec
      break;
    case SMELL_DELAY_FAN:
      LOG_PRINTLN(F("Smell finished"));
      smellPhase = SMELL_OFF;
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->restoreCurrentSpeed();
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(FAN_SPEED_MIN);
      break;
  }
}

void SmellController::releaseSmell(SmellController::SMELL_LOCATION location, uint8_t intensity) {
  if (intensity>10) return;
  if (smellPhase!=SMELL_OFF) {
    LOG_PRINTLN(F("Smell ongoing"));
    return;
  }
  
  currentIntensity = intensity;
  currentLocation = location;
  
  LOG_PRINT(F("Setting smell at "));
  LOG_PRINT(location);
  LOG_PRINT(F(", intensity "));
  LOG_PRINTLN(intensity);

  smellPhase = SMELL_INIT_FAN;
  LOG_PRINTLN(F("Init fan"));

  taskManager->getTask<FanController*>(FAN_CONTROLLER)->saveCurrentSpeed();
  taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(EMIT_SPEED_LEVEL);
  
  triggerUpdateDelay(500);    // give the fan 500 ms to turn on
}

void SmellController::onPropertyValueChange(uint8_t id, bool newValue, bool oldValue) {
  int v = newValue ? LOW : HIGH;
  
  switch(id) {
    case SMELL_LOCATION_LEFT:
      digitalWrite(PIN_MM_RELAY1, v);
      break;
    case SMELL_LOCATION_MIDDLE:
      digitalWrite(PIN_MM_RELAY2, v);
      break;
    case SMELL_LOCATION_RIGHT:
      digitalWrite(PIN_MM_RELAY3, v);
      break;
    default:
      // unknown id
      break;
  }
}
