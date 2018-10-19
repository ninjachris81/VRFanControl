#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>
#include "Pins.h"

#define FAN_SPEED_MAX 9
#define FAN_SPEED_MIN 0

class FanController : public AbstractTriggerTask, public Property<uint8_t>::ValueChangeListener {
public:
  FanController();

  void init();

  void update();

  void setSpeedLevel(uint8_t speedLevel);

  uint8_t getSpeedLevel();

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

  void saveCurrentSpeed();

  void restoreCurrentSpeed();

private:
  Property<uint8_t> fanSpeed;

  uint8_t savedFanSpeed;
};


#endif    //FANCONTROLLER_H
