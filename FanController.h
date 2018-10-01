#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>
#include "Pins.h"

#define SPEED_COUNT 2

#define SPEED_MAX 9
#define SPEED_MIN 0

class FanController : public AbstractTriggerTask, public Property<uint8_t>::ValueChangeListener {
public:
  FanController();

  enum SPEED_LOCATION {
    SPEED_LOCATION_INVALID = 0,
    SPEED_LOCATION_LEFT,
    SPEED_LOCATION_RIGHT
  };
  
  void init();

  void update();

  void setSpeedLevel(SPEED_LOCATION location, uint8_t speedLevel);

  uint8_t getSpeedLevel(SPEED_LOCATION location);

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

  void saveCurrentSpeeds();

  void restoreCurrentSpeeds();

private:
  Property<uint8_t> speeds[SPEED_COUNT];

  uint8_t savedSpeeds[SPEED_COUNT];
};


#endif    //FANCONTROLLER_H
