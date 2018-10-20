#ifndef COMMCONTROLLER_H
#define COMMCONTROLLER_H

#include <AbstractTask.h>
#include <SoftwareSerial.h>

#include "CommHandler.h"
#include "Pins.h"

class CommController : public AbstractTask, public CommHandler {
public:
  CommController();

  void init();

  void update();

  void sendPackage(uint8_t *data);

  void sendPackage(char cmd, char mod, uint8_t value);

private:
  SoftwareSerial* ss;
};


#endif // COMMCONTROLLER_H
