#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  LOG_PRINTLN(F("Available commands:"));
  //LOG_PRINTLN(F("sl[0-9] -> speed left"));
  //LOG_PRINTLN(F("sr[0-9] -> speed right"));
  //LOG_PRINTLN(F("sm[1-9999] -> smell"));
}

void CommController::update() {
  if (Serial.available()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    uint8_t data[DATA_PACKAGE_SIZE];
    for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
      int d = Serial.read();
      data[i] = d;
    }
    
    handlePackage(taskManager, data);
  }
}

