#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  LOG_PRINTLN(F("Available commands:"));
  LOG_PRINTLN(F("p -> ping"));
  LOG_PRINTLN(F("v[r|m|l][0...9] -> vapo"));
  LOG_PRINTLN(F("f[l|r][0...9] -> fan"));
  LOG_PRINTLN(F("s[f|b] -> seat fwd/back"));
}

void CommController::update() {
  if (Serial.available()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    uint8_t data[DATA_PACKAGE_SIZE];
    for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
      int d = Serial.read();
      if (d==-1 || d==10 || d==13) return;
      
      data[i] = d;
    }
    
    Serial.read();    // ignore
    
    handlePackage(taskManager, data);
  }
}
