#include "CommHandler.h"

#include <LogHelper.h>

#include "SmellController.h"
#include "FanController.h"
#include "CommController.h"
#include "LedController.h"

#include "Protocol.h"
#include "TaskIDs.h"
#include "Debug.h"
#include "InitialBroadcastSupport.h"

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

      if (isFirstPing) {
        isFirstPing = false;
        LOG_PRINTLN(F("BC First Ping"));

        for (uint8_t i=0;i<IBS_OBJECT_COUNT;i++) {
          InitialBroadcastSupport* obj = InitialBroadcastSupport::getObject(i);
          if (obj!=NULL) {
            obj->onInitialBroadcast();
          } else {
            LOG_PRINT(F("Break ibs at "));
            LOG_PRINTLN(i);
            break;
          }
        }
      }
      break;
    case CMD_VAPO: {
      SmellController::SMELL_LOCATION location = taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->resolveLocation(data[2]);
  
      if (location != SmellController::SMELL_LOCATION_INVALID) {
        taskManager->getTask<SmellController*>(SMELL_CONTROLLER)->releaseSmell(location, value);
      } else {
        LOG_PRINT(F("Invalid location "));
        LOG_PRINTLNF(data[2], HEX);
      }
      break;
    }
    case CMD_FAN:
      taskManager->getTask<FanController*>(FAN_CONTROLLER)->setSpeedLevel(value);
      break;
    case CMD_LED_COLOR: {
      LedController::LED_LOCATION location = taskManager->getTask<LedController*>(LED_CONTROLLER)->resolveLocation(data[2]);
  
      if (location != LedController::LED_LOCATION_INVALID) {
        if (location==LedController::LED_LOCATION_HEADLIGHTS_LEFT) {
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setColor(LedController::LED_LOCATION_HEADLIGHTS_LEFT, value);
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setColor(LedController::LED_LOCATION_HEADLIGHTS_RIGHT, value);
        } else {
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setColor(location, value);
        }
      } else {
        // invalid
        LOG_PRINT(F("Invalid location "));
        LOG_PRINTLNF(data[2], HEX);
      }
      break;
    }
    case CMD_LED_BRIGHTNESS: {
      LedController::LED_LOCATION location = taskManager->getTask<LedController*>(LED_CONTROLLER)->resolveLocation(data[2]);
  
      if (location != LedController::LED_LOCATION_INVALID) {
        if (location==LedController::LED_LOCATION_HEADLIGHTS_LEFT) {
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setBrightness(LedController::LED_LOCATION_HEADLIGHTS_LEFT, value);        
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setBrightness(LedController::LED_LOCATION_HEADLIGHTS_RIGHT, value);        
        } else {
          taskManager->getTask<LedController*>(LED_CONTROLLER)->setBrightness(location, value);
        }
      } else {
        // invalid
        LOG_PRINT(F("Invalid location "));
        LOG_PRINTLNF(data[2], HEX);
      }
      break;
    }
    default:
      LOG_PRINT(F("Unknown command "));
      LOG_PRINTLNF(data[1], HEX);
      break;
  }

  
}
