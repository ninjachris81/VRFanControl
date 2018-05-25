#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>

#define MOSFET_PIN_LEFT 9
#define MOSFET_PIN_RIGHT 10

#define SPEED_COUNT 2

#define SPEED_LEFT 0
#define SPEED_RIGHT 1

#define SPEED_MAX 9
#define SPEED_MIN 0

class FanController : public AbstractTriggerTask, public Property<uint8_t>::ValueChangeListener {
public:
  FanController();
  
  void init();

  void update();

  void setSpeedLevel(uint8_t index, uint8_t speedLevel);

  uint8_t getSpeedLevel(uint8_t index);

  void onPropertyValueChange(uint8_t id, uint8_t value);

private:
  Property<uint8_t> speeds[SPEED_COUNT];
  
};


#endif    //FANCONTROLLER_H
