#ifndef INITIAL_BROADCAST_SUPPORT_H
#define INITIAL_BROADCAST_SUPPORT_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define IBS_OBJECT_COUNT 10

class InitialBroadcastSupport {
public:
  virtual void onInitialBroadcast() = 0;

  static void init();

  static void registerObject(InitialBroadcastSupport* thisObject);

  static InitialBroadcastSupport* getObject(uint8_t index);

private:
  static InitialBroadcastSupport* objects[IBS_OBJECT_COUNT];
  static uint8_t currentCount;

};

#endif    // INITIAL_BROADCAST_SUPPORT_H
