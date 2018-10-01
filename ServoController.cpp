#include "ServoController.h"

#include <LogHelper.h>
#include "TaskIDs.h"

ServoController::ServoController() : AbstractIntervalTask(100) {
}

void ServoController::init() {
  servoDriver.begin();
  servoDriver.setPWMFreq(1600);
}

void ServoController::update() {
}

void ServoController::triggerAction(uint8_t index) {
  //double pulselength = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
  //servoDriver.setPWM(index, 0, pulselength);
}

int ServoController::getServoAngle(uint8_t index) {
  
}

