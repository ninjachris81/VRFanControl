#include "CommHandler.h"

#include <LogHelper.h>

#include "SeatController.h"
#include "CommController.h"
#include "WifiController.h"
#include "Protocol.h"
#include "TaskIDs.h"
#include "Debug.h"

CommHandler::CommHandler() {
}

void CommHandler::handlePackage(TaskManager* taskManager, uint8_t* data) {
  if (!data[0]==CMD_IDENTIFIER) return;

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
      sendPackage(CMD_PING_FB, 'X', 0);
      break;
    case CMD_PING_FB:
      taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->dbPingbackReceived();
      break;
    case CMD_VAPO:
      taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(data);
      break;
    case CMD_VAPO_FB:
      taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->notifyPackage(data);
      break;
    case CMD_FAN:
      taskManager->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(data);
      break;
    case CMD_FAN_FB:
      taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->notifyPackage(data);
      break;
    case CMD_LEVEL_FB:
      taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->notifyPackage(data);
      break;
    case CMD_MM_FB:
      taskManager->getTask<WifiController*>(WIFI_CONTROLLER)->notifyPackage(data);
      break;
    case CMD_SEAT: {
      SeatController::SEAT_DIRECTION direction = SeatController::SEAT_INVALID;
      
      switch(data[2]) {
        case CMD_FORWARD:
          direction = SeatController::SEAT_FORWARD;
          break;
        case CMD_BACKWARD:
          direction = SeatController::SEAT_BACKWARD;
          break;
      }

      if (direction!=SeatController::SEAT_INVALID) {
        taskManager->getTask<SeatController*>(SEAT_CONTROLLER)->moveSeat(direction);
      }

      break;
    }
    default:
      //LOG_PRINT(F("Unknown command "));
      //LOG_PRINTLNF(data[1], HEX);
      break;
  } 
}
