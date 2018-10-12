#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_COUNT 3

#define SERVOMIN 1800
#define SERVOMAX 2900

class ServoController : public AbstractTriggerTask {
public:
  ServoController();

  void init();

  void update();

  void triggerAction(uint8_t index);

private:
  Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver();
  uint8_t currentIndex;

};


#endif    //SERVOCONTROLLER_H
