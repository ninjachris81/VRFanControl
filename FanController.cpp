#include "FanController.h"

#include <LogHelper.h>

FanController::FanController() : AbstractTriggerTask() {
  
}

void FanController::init() {
  pinMode(PIN_MOSFET_LEFT, OUTPUT);
  pinMode(PIN_MOSFET_RIGHT, OUTPUT);

  analogWrite(PIN_MOSFET_LEFT, SPEED_MIN);
  analogWrite(PIN_MOSFET_RIGHT, SPEED_MIN);

  for (uint8_t i=0;i<SPEED_COUNT;i++) {
    speeds[i].registerValueChangeListener(this);
    speeds[i].init(i, 0);
  }
}

void FanController::update() {
}

void FanController::saveCurrentSpeeds() {
  for (uint8_t i=0;i<SPEED_COUNT;i++) savedSpeeds[i] = speeds[i].getValue();
}

void FanController::restoreCurrentSpeeds() {
  for (uint8_t i=0;i<SPEED_COUNT;i++) speeds[i].setValue(savedSpeeds[i]);
}

uint8_t FanController::getSpeedLevel(SPEED_LOCATION location) {
  return speeds[location].getValue();
}

void FanController::setSpeedLevel(SPEED_LOCATION location, uint8_t speedLevel) {
  if (speedLevel>SPEED_MAX) return;
  
  uint8_t speedValue = 0;

  switch(speedLevel) {
    case 0: 
      speedValue = 0;
      break;
    case 1:
      speedValue = 10;
      break;
    case 2:
      speedValue = 14;
      break;
    case 3:
      speedValue = 20;
      break;
    case 4:
      speedValue = 32;
      break;
    case 5:
      speedValue = 45;
      break;
    case 6:
      speedValue = 60;
      break;
    case 7:
      speedValue = 80;
      break;
    case 8:
      speedValue = 110;
      break;
    case 9:
      speedValue = 255;
      break;
  }
  
  speeds[location].setValue(speedValue);
}


void FanController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("Speed "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);
  
  if (id==SPEED_LOCATION_LEFT) analogWrite(PIN_MOSFET_LEFT, newValue);
  if (id==SPEED_LOCATION_RIGHT) analogWrite(PIN_MOSFET_RIGHT, newValue);
}

