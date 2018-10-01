#ifndef SERVOCONTROLLER_H
#define SERVOCONTROLLER_H

#include <AbstractIntervalTask.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVO_COUNT 3

#define SERVOMIN 0
#define SERVOMAX 1600

class ServoController : public AbstractIntervalTask {
public:
  ServoController();

  void init();

  void update();

  void triggerAction(uint8_t index);

  int getServoAngle(uint8_t index);
  
private:
  Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(0x40);

};


#endif    //SERVOCONTROLLER_H
