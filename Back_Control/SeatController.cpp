#include "SeatController.h"

#include <LogHelper.h>
#include "Pins.h"
#include "Protocol.h"
#include "TaskIDs.h"

SeatController::SeatController() : AbstractIntervalTask(SEAT_CHECK_INTERVAL) {
}

void SeatController::init() {
  // direction
  pinMode(PIN_SEAT_FORWARD, OUTPUT);
  pinMode(PIN_SEAT_BACKWARD, OUTPUT);

  // switches
  pinMode(PIN_SEAT_SWITCH_FORWARD, INPUT);
  pinMode(PIN_SEAT_SWITCH_BACKWARD, INPUT);

  // default values
  digitalWrite(PIN_SEAT_FORWARD, SEAT_RELAY_OFF);
  digitalWrite(PIN_SEAT_BACKWARD, SEAT_RELAY_OFF);

  currentMovement.init(PROP_CURRENT_MOVEMENT, SEAT_STOP);
  currentMovement.registerValueChangeListener(this);
  
  currentPosition.init(PROP_CURRENT_POSITION, SEAT_STOP);
  currentPosition.registerValueChangeListener(this);


  switchForward.init(PROP_SWITCH_FORWARD, SEAT_STOP);
  switchForward.registerValueChangeListener(this);
  
  switchBackward.init(PROP_SWITCH_BACKWARD, SEAT_STOP);
  switchBackward.registerValueChangeListener(this);
}

void SeatController::update() {
  switchForward.setValue(digitalRead(PIN_SEAT_SWITCH_FORWARD)==SEAT_RELAY_ON);
  switchBackward.setValue(digitalRead(PIN_SEAT_SWITCH_BACKWARD)==SEAT_RELAY_ON);

  if (switchForward.getValue() && switchBackward.getValue()) {
    currentPosition.setValue(SEAT_ERROR);    // error
  } else if (switchForward.getValue()) {
    currentPosition.setValue(SEAT_FORWARD);
  } else if (switchBackward.getValue()) {
    currentPosition.setValue(SEAT_BACKWARD);
  } else {
    currentPosition.setValue(SEAT_STOP);
  }
}

void SeatController::moveSeat(SeatController::SEAT_DIRECTION direction) {
  if (currentPosition.getValue()==direction) {
    LOG_PRINTLN(F("Already in position"));
    return;
  }
  
  currentMovement.setValue(direction);
}

void SeatController::onPropertyValueChange(uint8_t id, int8_t newValue, int8_t oldValue) {
  switch(id) {
    case PROP_CURRENT_MOVEMENT:
      LOG_PRINT(F("Seat movement "));
      LOG_PRINTLN(SEAT_MOVE_TO_STRING(newValue));
      
      switch(newValue) {
        case SEAT_STOP:
          digitalWrite(PIN_SEAT_FORWARD, SEAT_RELAY_OFF);
          digitalWrite(PIN_SEAT_BACKWARD, SEAT_RELAY_OFF);
          break;
        case SEAT_FORWARD:
          digitalWrite(PIN_SEAT_FORWARD, SEAT_RELAY_ON);
          digitalWrite(PIN_SEAT_BACKWARD, SEAT_RELAY_OFF);
          break;
        case SEAT_BACKWARD:
          digitalWrite(PIN_SEAT_FORWARD, SEAT_RELAY_OFF);
          digitalWrite(PIN_SEAT_BACKWARD, SEAT_RELAY_ON);
          break;
      }
      
      //taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->sendPackage(CMD_SEAT_MOVE_FB, MOD_NONE, newValue, true);
      break;
    case PROP_CURRENT_POSITION:
      LOG_PRINT(F("Seat position "));
      LOG_PRINTLN(SEAT_POS_TO_STRING(newValue));

      //taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->sendPackage(CMD_SEAT_POS_FB, MOD_NONE, newValue, true);

      break;
    case PROP_SWITCH_FORWARD:
      LOG_PRINT(F("Seat switch forward "));
      LOG_PRINTLN(newValue);
      
      //taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->sendPackage(CMD_SEAT_SWITCH_FB, MOD_FORWARD, newValue, true);
      if (newValue) currentMovement.setValue(SEAT_STOP);

      break;
    case PROP_SWITCH_BACKWARD:
      LOG_PRINT(F("Seat switch backward "));
      LOG_PRINTLN(newValue);
      
      //taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->sendPackage(CMD_SEAT_SWITCH_FB, MOD_BACKWARD, newValue, true);
      if (newValue) currentMovement.setValue(SEAT_STOP);
      
      break;
  }
}
