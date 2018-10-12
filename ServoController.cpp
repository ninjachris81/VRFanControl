#include "ServoController.h"

#include <LogHelper.h>
#include "TaskIDs.h"

ServoController::ServoController() : AbstractTriggerTask() {
}

void ServoController::init() {
  servoDriver.begin();
  servoDriver.setPWMFreq(1000);
}

void ServoController::update() {
  LOG_PRINT(F("Finish action "));
  LOG_PRINTLN(currentIndex);
  servoDriver.setPWM(currentIndex, 0, SERVOMIN);
}

void ServoController::triggerAction(uint8_t index) {
  LOG_PRINT(F("Trigger action "));
  LOG_PRINTLN(index);

  currentIndex = index;
  
  servoDriver.setPWM(index, 0, SERVOMAX);
  triggerUpdateDelay(500);
}
