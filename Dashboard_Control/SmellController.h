#ifndef SMELLCONTROLLER_H
#define SMELLCONTROLLER_H

#include <AbstractTriggerTask.h>
#include <Property.h>
#include "Pins.h"
#include "GlobalConstants.h"
#include "InitialBroadcastSupport.h"

#define EMIT_SPEED_LEVEL 5

class SmellController : public AbstractTriggerTask, public Property<bool>::ValueChangeListener, public InitialBroadcastSupport  {
public:
  SmellController();

  enum SMELL_PHASE {
    SMELL_OFF = 0,
    SMELL_INIT_FAN,
    SMELL_VAPO,
    SMELL_DELAY_FAN
  };

  enum SMELL_LOCATION {
    SMELL_LOCATION_INVALID = -1,
    SMELL_LOCATION_LEFT,
    SMELL_LOCATION_MIDDLE,
    SMELL_LOCATION_RIGHT
  };

  void init();

  void update();

  void onInitialBroadcast();

  void onPropertyValueChange(uint8_t id, bool newValue, bool oldValue);

  void releaseSmell(SMELL_LOCATION location, uint8_t intensity);
  
private:
  uint8_t currentIntensity;
  SMELL_LOCATION currentLocation = SMELL_LOCATION_INVALID;
  SMELL_PHASE smellPhase = SMELL_OFF;

  Property<bool> mmStates[MM_COUNT];
  
};


#endif    //SMELLCONTROLLER_H
