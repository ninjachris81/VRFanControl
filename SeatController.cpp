#include "SeatController.h"

#include <LogHelper.h>
#include "Pins.h"

SeatController::SeatController() : AbstractTriggerTask() {
}

void SeatController::init() {
  pinMode(PIN_SEAT_FORWARD, OUTPUT);
  pinMode(PIN_SEAT_BACKWARD, OUTPUT);

  digitalWrite(PIN_SEAT_FORWARD, LOW);
  digitalWrite(PIN_SEAT_BACKWARD, LOW);

  currentMovement.init(0, SEAT_INVALID);
  currentMovement.registerValueChangeListener(this);
  
  currentPosition.init(1, SEAT_INVALID);
}

void SeatController::update() {
  currentPosition.setValue(currentMovement.getValue());
  currentMovement.setValue(SEAT_INVALID);
}

void SeatController::moveSeat(SeatController::SEAT_DIRECTION direction) {
  if (currentPosition.getValue()==direction) {
    LOG_PRINTLN(F("Already in position"));
    return;
  }
  
  currentMovement.setValue(direction);
  triggerUpdateDelay(MOVEMENT_DURATION);
}

void SeatController::onPropertyValueChange(uint8_t id, int8_t newValue, int8_t oldValue) {
  switch(newValue) {
    case SEAT_INVALID:
      digitalWrite(PIN_SEAT_FORWARD, LOW);
      digitalWrite(PIN_SEAT_BACKWARD, LOW);
      break;
    case SEAT_FORWARD:
      digitalWrite(PIN_SEAT_FORWARD, HIGH);
      digitalWrite(PIN_SEAT_BACKWARD, LOW);
      break;
    case SEAT_BACKWARD:
      digitalWrite(PIN_SEAT_FORWARD, LOW);
      digitalWrite(PIN_SEAT_BACKWARD, HIGH);
      break;
  }
}
