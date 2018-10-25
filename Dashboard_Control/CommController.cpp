#include "CommController.h"

#include <LogHelper.h>
#include "Protocol.h"
#include "Debug.h"
#include "GlobalConstants.h"

CommController::CommController() : AbstractTask() {
  
}

void CommController::init() {
#ifdef ARDUINO_AVR_MEGA2560
  Serial1.begin(SERIAL_SPEED);
#else
  ss = new SoftwareSerial(PIN_COMM_RX, PIN_COMM_TX, false);
  ss->begin(SERIAL_SPEED);
#endif
}

void CommController::update() {
  if (serialAvailable()>=DATA_PACKAGE_SIZE+1) {    // last one is \r
    char c = serialPeek();
    if (c==CMD_IDENTIFIER) {
      uint8_t data[DATA_PACKAGE_SIZE];
      for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
        data[i] = serialRead();
      }

      serialRead();   // remove last \n

      c = serialPeek();
      if (c==10 || c==13) serialRead();
      
      handlePackage(taskManager, data);
    } else {
      // remove garbage
      char c = serialRead();

      if (c!=10 && c!=13) {
        LOG_PRINT(F("Chunk "));
        LOG_PRINTLNF(c, HEX);
      }
    }
  }
}

int CommController::serialAvailable() {
#ifdef ARDUINO_AVR_MEGA2560
  return Serial1.available();
#else
  return ss->available();
#endif
}

char CommController::serialPeek() {
#ifdef ARDUINO_AVR_MEGA2560
  return Serial1.peek();
#else
  return ss->peek();
#endif
}

int CommController::serialRead() {
#ifdef ARDUINO_AVR_MEGA2560
  return Serial1.read();
#else
  return ss->read();
#endif
}

void CommController::sendPackage(char cmd, char mod, uint8_t value) {
  char c = value + '0';

  if ((cmd!=CMD_PING && cmd!=CMD_PING_FB) || PRINT_PING) {
    LOG_PRINT(F("Sending package "));
    LOG_PRINT(cmd);
    LOG_PRINT(mod);
    LOG_PRINTLN(c);
  }
  
#ifdef ARDUINO_AVR_MEGA2560
  Serial1.print(CMD_IDENTIFIER);
  Serial1.print(cmd);
  Serial1.print(mod);
  Serial1.println(c);
#else 
  ss->print(CMD_IDENTIFIER);
  ss->print(cmd);
  ss->print(mod);
  ss->println(c);
#endif
}
