#ifndef SMELLCONTROLLER_H
#define SMELLCONTROLLER_H

#include <AbstractTriggerTask.h>

#define PIN_SMELL 3

class SmellController : public AbstractTriggerTask {
public:
  SmellController();

  enum SMELL_PHASE {
    SMELL_OFF = 0,
    SMELL_INIT_FAN,
    SMELL_VAPO,
    SMELL_DELAY_FAN
  };
  
  void init();

  void update();

  void releaseSmell(uint16_t intensity);
  
private:
  uint8_t orgSpeedLevel;
  uint16_t currentIntensity;
  SMELL_PHASE smellPhase = SMELL_OFF;
  
};


#endif    //SMELLCONTROLLER_H
