#include "FanController.h"

#include <LogHelper.h>

FanController::FanController() : AbstractTriggerTask() {
  
}

void FanController::init() {
  TCCR1B = TCCR1B & 0b11111000 | 0x01; // Setzt Timer1 (Pin 9 und 10) auf 31300Hz  
  
  pinMode(MOSFET_PIN_LEFT, OUTPUT);
  pinMode(MOSFET_PIN_RIGHT, OUTPUT);

  analogWrite(MOSFET_PIN_LEFT, 0);
  analogWrite(MOSFET_PIN_RIGHT, 0);

  for (uint8_t i=0;i<SPEED_COUNT;i++) {
    speeds[i].registerValueChangeListener(this);
    speeds[i].init(i, 0);
    boosts[i] = false;
  }
}

void FanController::update() {
  for (uint8_t i=0;i<SPEED_COUNT;i++) {
    if (boosts[i]) {
      boosts[i] = false;
      speeds[i].fireChangeEvent();
    }
  }
}

uint8_t FanController::getSpeedLevel(uint8_t index) {
  return speeds[index].getValue();
}

void FanController::setSpeedLevel(uint8_t index, uint8_t speedLevel) {
  if (speedLevel>SPEED_MAX) return;
  
  index = constrain(index, 0, SPEED_COUNT-1);
  uint8_t speedValue = 0;

  switch(speedLevel) {
    case 0: 
      speedValue = 0;
      break;
    case 1:
      speedValue = 36;
      break;
    case 2:
      speedValue = 38;
      break;
    case 3:
      speedValue = 40;
      break;
    case 4:
      speedValue = 45;
      break;
    case 5:
      speedValue = 50;
      break;
    case 6:
      speedValue = 55;
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
  
  speeds[index].setValue(speedValue);
}


void FanController::onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue) {
  LOG_PRINT(F("Speed "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(newValue);

  if (oldValue==0) {
    boosts[id] = true;
    newValue = 100;
    triggerUpdateDelay(100);
  }
  
  if (id==SPEED_LEFT) analogWrite(MOSFET_PIN_LEFT, newValue);
  if (id==SPEED_RIGHT) analogWrite(MOSFET_PIN_RIGHT, newValue);
}
