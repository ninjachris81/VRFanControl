#ifndef COMMHANDLER_H
#define COMMHANDLER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <TaskManager.h>

class CommHandler {
public:
  CommHandler();

  void handlePackage(TaskManager* taskManager, uint8_t* data);

  virtual void sendPackage(char cmd, char mod, uint8_t value) = 0;
};

#endif    //COMMHANDLER_H
