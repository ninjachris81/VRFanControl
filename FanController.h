#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>
#include "Pins.h"

#define FAN_COUNT 2

#define FAN_SPEED_MAX 9
#define FAN_SPEED_MIN 0

class FanController : public AbstractTriggerTask, public Property<uint8_t>::ValueChangeListener {
public:
  FanController();

  enum SPEED_LOCATION {
    SPEED_LOCATION_INVALID = -1,
    SPEED_LOCATION_LEFT = 0,
    SPEED_LOCATION_RIGHT,
    SPEED_LOCATION_SMELL
  };
  
  void init();

  void update();

  void setSpeedLevel(SPEED_LOCATION location, uint8_t speedLevel);

  uint8_t getSpeedLevel(SPEED_LOCATION location);

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

  void saveCurrentSpeeds();

  void restoreCurrentSpeeds();

private:
  Property<uint8_t> speeds[FAN_COUNT];

  uint8_t savedSpeeds[FAN_COUNT];
};


#endif    //FANCONTROLLER_H
