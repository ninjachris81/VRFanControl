#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"
#include "Debug.h"
#include "GlobalConstants.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  COMM_SERIAL.begin(SERIAL_SPEED);
}

void CommController::update() {
  if (COMM_SERIAL.available()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    char c = COMM_SERIAL.peek();
    if (c==CMD_IDENTIFIER) {
      uint8_t data[DATA_PACKAGE_SIZE];
      for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
        data[i] = COMM_SERIAL.read();
      }

      COMM_SERIAL.read();   // remove last \n

      c = COMM_SERIAL.peek();
      if (c==10 || c==13) COMM_SERIAL.read();
      
      handlePackage(taskManager, data);
    } else {
      // remove garbage
      char c = COMM_SERIAL.read();

      if (c!=10 && c!=13) {
        LOG_PRINT(F("Chunk "));
        LOG_PRINTLNF(c, HEX);
      }
    }
  }
}

void CommController::sendPackage(char cmd, char mod, uint8_t value) {
  char c = value + '0';

  if ((cmd!=CMD_PING && cmd!=CMD_PING_FB) || PRINT_PING) {
    LOG_PRINT(F("Sending package "));
    LOG_PRINT(cmd);
    LOG_PRINT(mod);
    LOG_PRINTLN(c);
  }
  
  Serial1.print(CMD_IDENTIFIER);
  Serial1.print(cmd);
  Serial1.print(mod);
  Serial1.println(c);
}
