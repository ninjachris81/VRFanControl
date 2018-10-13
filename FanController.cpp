#include "FanController.h"

#include <LogHelper.h>

FanController::FanController() : AbstractTriggerTask() {
  
}

void FanController::init() {
  pinMode(PIN_MOSFET_LEFT, OUTPUT);
  pinMode(PIN_MOSFET_RIGHT, OUTPUT);

  analogWrite(PIN_MOSFET_LEFT, 0);
  analogWrite(PIN_MOSFET_RIGHT, 0);

  for (uint8_t i=0;i<FAN_COUNT;i++) {
    speeds[i].registerValueChangeListener(this);
    speeds[i].init(i, 0);
  }
}

void FanController::update() {
}

void FanController::saveCurrentSpeeds() {
  for (uint8_t i=0;i<FAN_COUNT;i++) savedSpeeds[i] = speeds[i].getValue();
}

void FanController::restoreCurrentSpeeds() {
  for (uint8_t i=0;i<FAN_COUNT;i++) speeds[i].setValue(savedSpeeds[i]);
}

uint8_t FanController::getSpeedLevel(SPEED_LOCATION location) {
  return speeds[location].getValue();
}

void FanController::setSpeedLevel(SPEED_LOCATION location, uint8_t speedLevel) {
  speeds[location].setValue(constrain(speedLevel, FAN_SPEED_MIN, FAN_SPEED_MAX));
}

void FanController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("Speed "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);

  int pwmValue = 200 + map(pow(newValue, 2), 0, 81, 0, 824);
  
  if (id==SPEED_LOCATION_LEFT) analogWrite(PIN_MOSFET_LEFT, pwmValue);
  if (id==SPEED_LOCATION_RIGHT) analogWrite(PIN_MOSFET_RIGHT, pwmValue);
}
