#ifndef SEATCONTROLLER_H
#define SEATCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>

#define MOVEMENT_DURATION 10000

class SeatController : public AbstractTriggerTask, public Property<int8_t>::ValueChangeListener {
public:
  SeatController();

  enum SEAT_DIRECTION {
    SEAT_INVALID = -1,
    SEAT_FORWARD = 0,
    SEAT_BACKWARD
  };

  void onPropertyValueChange(uint8_t id, int8_t newValue, int8_t oldValue);
  
  void init();

  void update();

  void moveSeat(SEAT_DIRECTION direction);
private:
  Property<int8_t> currentMovement;
  Property<int8_t> currentPosition;
};

#endif    //SEATCONTROLLER_H
