#include "CommHandler.h"

#include <LogHelper.h>

#include "SmellController.h"
#include "FanController.h"
#include "Protocol.h"
#include "TaskIDs.h"

CommHandler::CommHandler() {
}

void CommHandler::handlePackage(TaskManager* taskManager, uint8_t* data) {
  
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
          break;
      }
      
      if (location != SmellController::SMELL_LOCATION_INVALID) {
        taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->releaseSmell(location, data[2]);
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
          break;
      }
      
      if (location != FanController::SPEED_LOCATION_INVALID) {
        taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(location, data[2]);
      }
      break;
    }
    default:
      LOG_PRINT(F("Unknown command "));
      LOG_PRINTLN(data[0]);
      break;
  }



  
}

