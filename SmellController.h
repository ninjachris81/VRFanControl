#ifndef SMELLCONTROLLER_H
#define SMELLCONTROLLER_H

#include <AbstractTriggerTask.h>
#include "Pins.h"

#define EMIT_SPEED_LEVEL 5

class SmellController : public AbstractTriggerTask {
public:
  SmellController();

  enum SMELL_PHASE {
    SMELL_OFF = 0,
    SMELL_INIT_FAN,
    SMELL_VAPO,
    SMELL_DELAY_FAN
  };

  enum SMELL_LOCATION {
    SMELL_LOCATION_INVALID = 0,
    SMELL_LOCATION_LEFT,
    SMELL_LOCATION_MIDDLE,
    SMELL_LOCATION_RIGHT
  };

  void init();

  void update();

  void releaseSmell(SMELL_LOCATION location, uint8_t count);
  
private:
  uint8_t currentEmitCount = 0;
  SMELL_LOCATION currentLocation = SMELL_LOCATION_INVALID;
  SMELL_PHASE smellPhase = SMELL_OFF;
  
};


#endif    //SMELLCONTROLLER_H
