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
};

#endif    //COMMHANDLER_H
