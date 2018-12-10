#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"
#include "Debug.h"
#include "GlobalConstants.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
  //ss = new SoftwareSerial(PIN_COMM_RX, PIN_COMM_TX, false);
  //ss->begin(SERIAL_SPEED);
  Serial.begin(SERIAL_SPEED);
}

void CommController::update() {
  if (Serial.available()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    char c = Serial.peek();
    if (c==CMD_IDENTIFIER) {
      uint8_t data[DATA_PACKAGE_SIZE];
      for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
        data[i] = Serial.read();
      }

      Serial.read();   // remove last \n

      c = Serial.peek();
      if (c==10 || c==13) Serial.read();
      
      handlePackage(taskManager, data);
    } else {
      // remove garbage
      char c = Serial.read();
      
      if (c!=10 && c!=13) {
        LOG_PRINT(F("Chunk "));
        LOG_PRINTLNF(c, HEX);
      }
    }
  }
}

void CommController::sendPackage(uint8_t *data) {
  sendPackage(data[1], data[2], data[3]);
}

void CommController::sendPackage(char cmd, char mod, uint8_t value) {
  char c = value + '0';

  if ((cmd!=CMD_PING && cmd!=CMD_PING_FB) || PRINT_PING) {
    LOG_PRINT(F("Sending package "));
    LOG_PRINT(cmd);
    LOG_PRINT(mod);
    LOG_PRINTLN(c);
  }

  Serial.print(CMD_IDENTIFIER);
  Serial.print(cmd);
  Serial.print(mod);
  Serial.println(c);
}
