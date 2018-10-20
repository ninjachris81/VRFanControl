#include "LevelController.h"

#include <LogHelper.h>

#include "CommController.h"
#include "TaskIDs.h"
#include "Protocol.h"

LevelController::LevelController() : AbstractIntervalTask(LEVEL_INTERVAL_MS) {
  
}

void LevelController::init() {
  pinMode(PIN_MM_LEVEL1, INPUT);
  pinMode(PIN_MM_LEVEL2, INPUT);
  pinMode(PIN_MM_LEVEL3, INPUT);

  for (uint8_t i=0;i<LEVEL_COUNT;i++) {
    levels[i].init(i, 0);
    levels[i].registerValueChangeListener(this);
  }
}

void LevelController::update() {
  levels[0].setValue(analogToLevel(analogRead(PIN_MM_LEVEL1)));
  levels[1].setValue(analogToLevel(analogRead(PIN_MM_LEVEL2)));
  levels[2].setValue(analogToLevel(analogRead(PIN_MM_LEVEL3)));
}

uint8_t LevelController::analogToLevel(uint16_t analogValue) {
  analogValue = min(max(analogValue, LEVEL_MIN), LEVEL_MAX);
  return map(analogValue, LEVEL_MIN, LEVEL_MAX, 9, 0);
}

void LevelController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_LEVEL_FB, id + '0', newValue);
}
