#ifndef COMMCONTROLLER_H
#define COMMCONTROLLER_H

#include <AbstractTask.h>
#ifdef ARDUINO_AVR_MEGA2560
#else
  #include <SoftwareSerial.h>
#endif
#include "CommHandler.h"
#include "Pins.h"

class CommController : public AbstractTask, public CommHandler {
public:
  CommController();

  void init();

  void update();

  void sendPackage(char cmd, char mod, uint8_t value);
private:
#ifdef ARDUINO_AVR_MEGA2560
#else
  SoftwareSerial* ss;
#endif

  int serialAvailable();
  char serialPeek();
  int serialRead();
};


#endif // COMMCONTROLLER_H
