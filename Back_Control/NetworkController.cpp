#include "NetworkController.h"

#include <LogHelper.h>
#include "Protocol.h"
#include "TaskIDs.h"
#include "CommController.h"

#include "Debug.h"

#include <ArduinoJson.h>

NetworkController* NetworkController::mInstance;
//WebSocketsServer* NetworkController::wsServer;
//EthernetClient NetworkController::activeClient;
//uint64_t NetworkController::lastDBPing = 0;

NetworkController::NetworkController() : AbstractTask() {
  mInstance = this;
}

NetworkController* NetworkController::instance() {
  return mInstance;
}

void NetworkController::init() {
  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };
  //IPAddress ip(192, 168, 0, 1);

  LOG_PRINTLN(F("Ethernet start"));
  
  Ethernet.begin(mac/*, ip*/);

/*
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    LOG_PRINTLN(F("Ethernet HW fail"));
  }
  
  if (Ethernet.linkStatus() == LinkOFF) {
    LOG_PRINTLN(F("Ethernet cable is not connected."));
  }
  */

/*
  wsServer = new WebSocketsServer(81);
  wsServer->begin();
  wsServer->onEvent(&NetworkController::onWebSocketEvent);
  */

  dataServer = new EthernetUDP();

  LOG_PRINTLN(F("Dataserver start"));
  dataServer->begin(8080);
  LOG_PRINT(F("Local IP: "));
  LOG_PRINTLN(Ethernet.localIP());
}

void NetworkController::update() {
  
  if (last_dhcp==0 || millis() - last_dhcp>DHCP_TIMEOUT_MS) {
    LOG_PRINTLN(F("DHCP maintain"));
    Ethernet.maintain();
    last_dhcp = millis();
  }

  int packetSize = dataServer->parsePacket();
  if (packetSize) {
    uint8_t data[DATA_PACKAGE_SIZE];
    int readDataSize = dataServer->read(data, DATA_PACKAGE_SIZE);
    if (readDataSize>=DATA_PACKAGE_SIZE) {
      handlePackage(taskManager, data);
      int remaining = dataServer->available();
      if (remaining>0) {
        LOG_PRINT(F("Remaining bytes: "));
        LOG_PRINTLN(remaining);
      }
    }
  }

  if (millis() - dbPingTrigger > DB_PING_INTERVAL) {
    dbPingTrigger = millis();
    instance()->getTaskManager()->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_PING, MOD_NONE, 0);
  }
}

/*
void NetworkController::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            LOG_PRINTLN(F("ws disconnected"));
            break;
        case WStype_CONNECTED:
            LOG_PRINTLN(F("ws connected"));
            // send message to client
            //wsServer->sendTXT(num, "Connected");
            break;
        case WStype_TEXT:
          if (strcmp((char*)payload, "refresh")==0) {
            refreshData();
          } else if (length==DATA_PACKAGE_SIZE) {
            if (payload[0]==CMD_IDENTIFIER) {
              LOG_PRINTLN(F("WS package"));
              payload[3] = payload[3] - '0';

              switch(payload[1]) {
                case CMD_SEAT:
                  instance()->handlePackage(instance()->getTaskManager(), payload);
                  break;
                default:
                  instance()->getTaskManager()->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(payload);
                  break;
              }
            } 
          }
          break;
        case WStype_BIN:
          // not supported
          //USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
          //hexdump(payload, length);

          // send message to client
          // webSocket.sendBIN(num, payload, length);
          break;
    }
}

void NetworkController::refreshData() {
  StaticJsonBuffer<512> jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();

  // active client
  if (activeClient && activeClient.connected()) {
    //array.add(activeClient.remoteIP());
  } else {
    array.add("-");
  }

  // ap clients
  //array.add(WiFi.softAPgetStationNum());

  // ws clients
  //array.add(wsServer->connectedClients());

  // uptime
  array.add((int)(millis()/1000));

  // ap ip
  //array.add(WiFi.softAPIP().toString());

  // sn rate
  //array.add(WiFi.RSSI());

  // db pingback
  array.add((int)(lastDBPing/1000));

  // free heap
  //array.add(ESP.getFreeHeap());

  String buf;
  array.printTo(buf);  
  wsServer->broadcastTXT(buf);
}

void NetworkController::notifyPackage(uint8_t *data) {
  LOG_PRINTLN(F("Notify package"));
  if (checkUTF8(data, DATA_PACKAGE_SIZE)) {
    //wsServer->broadcastTXT(data, DATA_PACKAGE_SIZE);
  } else {
    LOG_PRINTLN(F("Invalid UTF-8"));
  }
}

void NetworkController::dbPingbackReceived() {
  if (PRINT_PING) {
    LOG_PRINTLN(F("DB ping received"));
  }

  lastDBPing = millis();
}*/

void NetworkController::sendPackage(uint8_t *data) {
  sendPackage(data, false);
}

void NetworkController::sendPackage(uint8_t *data, const bool notify) {
  dataServer->beginPacket(dataServer->remoteIP(), dataServer->remotePort());
  dataServer->write(data, DATA_PACKAGE_SIZE);
  dataServer->endPacket();

  if (notify) {
    //notifyPackage(data);
    data[3] = data[3] - '0';
    instance()->getTaskManager()->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(data);
  }
}

void NetworkController::sendPackage(char cmd, char mod, uint8_t value) {
  sendPackage(cmd, mod, value, false);
}

void NetworkController::sendPackage(char cmd, char mod, uint8_t value, const bool notify) {
  uint8_t package[DATA_PACKAGE_SIZE];

  package[0] = CMD_IDENTIFIER;
  package[1] = cmd;
  package[2] = mod;
  package[3] = value + '0';

  sendPackage(package, notify);
}

/*
bool NetworkController::checkUTF8(uint8_t *string, int length) {
    int c,i,ix,n,j;
    for (i=0, ix=length; i < ix; i++)
    {
        c = (unsigned char) string[i];
        //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
        if (0x00 <= c && c <= 0x7f) n=0; // 0bbbbbbb
        else if ((c & 0xE0) == 0xC0) n=1; // 110bbbbb
        else if ( c==0xed && i<(ix-1) && ((unsigned char)string[i+1] & 0xa0)==0xa0) return false; //U+d800 to U+dfff
        else if ((c & 0xF0) == 0xE0) n=2; // 1110bbbb
        else if ((c & 0xF8) == 0xF0) n=3; // 11110bbb
        //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
        //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
        else return false;
        for (j=0; j<n && i<ix; j++) { // n bytes matching 10bbbbbb follow ?
            if ((++i == ix) || (( (unsigned char)string[i] & 0xC0) != 0x80))
                return false;
        }
    }
    return true;
}*/
