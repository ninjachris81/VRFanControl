#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
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

void CommController::sendPackage(char cmd, char mod, uint8_t value) {
  Serial.print(CMD_IDENTIFIER);
  Serial.print(cmd);
  Serial.print(mod);
  char c = value + '0';
  Serial.println(c);
}
