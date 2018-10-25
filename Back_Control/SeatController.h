#ifndef SEATCONTROLLER_H
#define SEATCONTROLLER_H

#include <AbstractIntervalTask.h>
#include <Property.h>

#define PROP_CURRENT_MOVEMENT 0
#define PROP_CURRENT_POSITION 1

#define PROP_SWITCH_FORWARD 2
#define PROP_SWITCH_BACKWARD 3

#define SEAT_CHECK_INTERVAL 100

class SeatController : public AbstractIntervalTask, public Property<int8_t>::ValueChangeListener {
public:
  SeatController();

  enum SEAT_DIRECTION {
    SEAT_STOP = 0,
    SEAT_FORWARD,
    SEAT_BACKWARD,
    SEAT_ERROR
  };

  void onPropertyValueChange(uint8_t id, int8_t newValue, int8_t oldValue);
  
  void init();

  void update();

  void moveSeat(SEAT_DIRECTION direction);
private:
  Property<int8_t> currentMovement;
  Property<int8_t> currentPosition;

  Property<int8_t> switchForward;
  Property<int8_t> switchBackward;
};

#endif    //SEATCONTROLLER_H
