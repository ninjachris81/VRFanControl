#ifndef LEVELCONTROLLER_H
#define LEVELCONTROLLER_H

#include <AbstractIntervalTask.h>
#include <Property.h>

#include "Pins.h"

#define LEVEL_COUNT 3

#define LEVEL_MIN 350
#define LEVEL_MAX 700

#define LEVEL_INTERVAL_MS 1000

class LevelController : public AbstractIntervalTask, public Property<uint8_t>::ValueChangeListener {
public:
  LevelController();

  void init();

  void update();

  void onPropertyValueChange(uint8_t id, uint8_t newValue, uint8_t oldValue);

private:
  Property<uint8_t> levels[LEVEL_COUNT];

  uint8_t analogToLevel(uint16_t analogValue);

};


#endif    //LEVELCONTROLLER_H
