#include "FanController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "CommController.h"
#include "Protocol.h"

FanController::FanController() : AbstractTriggerTask() {
  
}

void FanController::init() {
  pinMode(PIN_MOSFET_FAN, OUTPUT);

  analogWrite(PIN_MOSFET_FAN, 0);

  fanSpeed.registerValueChangeListener(this);
  fanSpeed.init(0, 0);
}

void FanController::update() {
}

void FanController::onInitialBroadcast() {
  taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_FAN_FB, MOD_NONE, fanSpeed.getValue());
}

void FanController::saveCurrentSpeed() {
  savedFanSpeed = fanSpeed.getValue();
  LOG_PRINT(F("Saved speed "));
  LOG_PRINTLN(savedFanSpeed);
}

void FanController::restoreCurrentSpeed() {
  LOG_PRINT(F("Restoring speed "));
  LOG_PRINTLN(savedFanSpeed);
  fanSpeed.setValue(savedFanSpeed);
}

uint8_t FanController::getSpeedLevel() {
  return fanSpeed.getValue();
}

void FanController::setSpeedLevel(uint8_t speedLevel) {
  fanSpeed.setValue(constrain(speedLevel, FAN_SPEED_MIN, FAN_SPEED_MAX));
}

void FanController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("Speed "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);

  //int pwmValue = 200 + map(pow(newValue, 2), 0, 81, 0, 824);
  //int pwmValue = map(newValue, 0, 9, 0, 660);
  int pwmValue = newValue * 4;
  analogWrite(PIN_MOSFET_FAN, pwmValue);

  LOG_PRINT(F("Speed Raw "));
  LOG_PRINTLN(pwmValue);
  
  taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_FAN_FB, MOD_NONE, newValue);
}
