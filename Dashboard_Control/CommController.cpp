#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"
#include "Debug.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  ss = new SoftwareSerial(PIN_COMM_RX, PIN_COMM_TX, false);
  ss->begin(9600);
}

void CommController::update() {
  if (ss->available()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    char c = ss->peek();
    if (c==CMD_IDENTIFIER) {
      uint8_t data[DATA_PACKAGE_SIZE];
      for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
        data[i] = ss->read();
      }

      ss->read();   // remove last \n

      c = ss->peek();
      if (c==10 || c==13) ss->read();
      
      handlePackage(taskManager, data);
    } else {
      // remove garbage
      char c = ss->read();

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
  
  ss->print(CMD_IDENTIFIER);
  ss->print(cmd);
  ss->print(mod);
  ss->println(c);
}
