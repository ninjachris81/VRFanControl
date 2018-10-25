#include "InitialBroadcastSupport.h"

uint8_t InitialBroadcastSupport::currentCount = 0;
InitialBroadcastSupport* InitialBroadcastSupport::objects[IBS_OBJECT_COUNT];

void InitialBroadcastSupport::init() {
  for (uint8_t i=0;i<IBS_OBJECT_COUNT;i++) {
    objects[i] = NULL;
  }
}

void InitialBroadcastSupport::registerObject(InitialBroadcastSupport* thisObject) {
  objects[currentCount] = thisObject;
  currentCount++;
}

InitialBroadcastSupport* InitialBroadcastSupport::getObject(uint8_t index) {
  return objects[index];
}
