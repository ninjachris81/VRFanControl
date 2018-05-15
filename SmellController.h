#ifndef SMELLCONTROLLER_H
#define SMELLCONTROLLER_H

#include <AbstractTriggerTask.h>

#define PIN_SMELL 3
#define TIME_FACTOR 500

class SmellController : public AbstractTriggerTask {
public:
  SmellController();
  
  void init();

  void update();

  void releaseSmell(uint8_t intensity);

private:
  
  
};


#endif    //SMELLCONTROLLER_H
