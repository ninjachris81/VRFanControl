#ifndef COMMCONTROLLER_H
#define COMMCONTROLLER_H

#include <AbstractTask.h>
#include "CommHandler.h"
#include "Pins.h"

#define COMM_SERIAL Serial

class CommController : public AbstractTask, public CommHandler {
public:
  CommController();

  void init();

  void update();

  void sendPackage(char cmd, char mod, uint8_t value);
};


#endif // COMMCONTROLLER_H
