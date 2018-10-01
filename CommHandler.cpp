#include "CommHandler.h"

#include <LogHelper.h>

#include "SmellController.h"
#include "FanController.h"
#include "Protocol.h"
#include "TaskIDs.h"

CommHandler::CommHandler() {
}

void CommHandler::handlePackage(TaskManager* taskManager, uint8_t* data) {
  uint8_t value = data[2] - '0';

  LOG_PRINT(F("Data: "));
  for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
    LOG_PRINT(String((char)data[i]) + " ");
  }
  LOG_PRINTLN("");
  
  switch(data[0]) {
    case CMD_PING:
      // do nothing
      break;
    case CMD_SMELL: {
      SmellController::SMELL_LOCATION location = SmellController::SMELL_LOCATION_INVALID;
  
      switch(data[1]) {
        case CMD_LEFT:
          location = SmellController::SMELL_LOCATION_LEFT;
          break;
        case CMD_MIDDLE:
          location = SmellController::SMELL_LOCATION_MIDDLE;
          break;
        case CMD_RIGHT:
          location = SmellController::SMELL_LOCATION_RIGHT;
          break;
        default:
          // invalid
          LOG_PRINT(F("Invalid location "));
          LOG_PRINTLNF(data[1], HEX);
          break;
      }
      
      if (location != SmellController::SMELL_LOCATION_INVALID) {
        taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->releaseSmell(location, value);
      }
      break;
    }
    case CMD_FAN: {
      FanController::SPEED_LOCATION location = FanController::SPEED_LOCATION_INVALID;
  
      switch(data[1]) {
        case CMD_LEFT:
          location = FanController::SPEED_LOCATION_LEFT;
          break;
        case CMD_RIGHT:
          location = FanController::SPEED_LOCATION_RIGHT;
          break;
        default:
          // invalid
          LOG_PRINT(F("Invalid location "));
          LOG_PRINTLNF(data[1], HEX);
          break;
      }
      
      if (location != FanController::SPEED_LOCATION_INVALID) {
        taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(location, value);
      }
      break;
    }
    default:
      LOG_PRINT(F("Unknown command "));
      LOG_PRINTLNF(data[0], HEX);
      break;
  }

  
}
