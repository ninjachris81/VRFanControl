#include "CommHandler.h"

#include <LogHelper.h>

#include "SmellController.h"
#include "FanController.h"
#include "CommController.h"

#include "Protocol.h"
#include "TaskIDs.h"
#include "Debug.h"

CommHandler::CommHandler() {
}

void CommHandler::handlePackage(TaskManager* taskManager, uint8_t* data) {
  if (data[0]!=CMD_IDENTIFIER) {
    LOG_PRINT(F("Invalid identifier "));
    LOG_PRINTLN(data[0]);
    return;
  }
  
  uint8_t value = data[3] - '0';

  if ((data[1]!=CMD_PING && data[1]!=CMD_PING_FB) || PRINT_PING) {
    LOG_PRINT(F("Received: "));
    for (uint8_t i=1;i<DATA_PACKAGE_SIZE;i++) {
      LOG_PRINT(String((char)data[i]) + " ");
    }
    LOG_PRINTLN("");
  }
  
  switch(data[1]) {
    case CMD_PING:
      sendPackage(CMD_PING_FB, MOD_NONE, 0);
      break;
    case CMD_VAPO: {
      SmellController::SMELL_LOCATION location = SmellController::SMELL_LOCATION_INVALID;
  
      switch(data[2]) {
        case MOD_LEFT:
          location = SmellController::SMELL_LOCATION_LEFT;
          break;
        case MOD_MIDDLE:
          location = SmellController::SMELL_LOCATION_MIDDLE;
          break;
        case MOD_RIGHT:
          location = SmellController::SMELL_LOCATION_RIGHT;
          break;
        default:
          // invalid
          LOG_PRINT(F("Invalid location "));
          LOG_PRINTLNF(data[2], HEX);
          break;
      }
      
      if (location != SmellController::SMELL_LOCATION_INVALID) {
        taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->releaseSmell(location, value);
      }
      break;
    }
    case CMD_FAN:
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(value);
      break;
    default:
      LOG_PRINT(F("Unknown command "));
      LOG_PRINTLNF(data[1], HEX);
      break;
  }

  
}
