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
  }
}

void FanController::update() {
}

void FanController::setSpeedLevel(uint8_t index, uint8_t speedLevel) {
  index = constrain(index, 0, SPEED_COUNT-1);
  uint8_t speed = map(speedLevel, 0, 9, 0, 255);
  speeds[index].setValue(speed);
}


void FanController::onPropertyValueChange(uint8_t id, uint8_t value) {
  LOG_PRINT(F("Speed "));
  LOG_PRINT(id);
  LOG_PRINT(": ");
  LOG_PRINTLN(value);
  
  if (id==SPEED_LEFT) analogWrite(MOSFET_PIN_LEFT, value);
  if (id==SPEED_RIGHT) analogWrite(MOSFET_PIN_RIGHT, value);
}

