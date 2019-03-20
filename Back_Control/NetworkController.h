#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include "AbstractTask.h"
#include <SPI.h>
#include <Ethernet.h>
#include "CommHandler.h"
//#include <WebSocketsServer.h>

#define DHCP_TIMEOUT_MS 120000

#define DB_PING_INTERVAL 4000

#define HOST_NAME "FAWControl"

class NetworkController : public AbstractTask, public CommHandler {
public:
  NetworkController();

  void init();

  void update();

  bool useInitWDT() {
    return false;
  }

  void notifyPackage(uint8_t *data);

  void sendPackage(uint8_t *data);

  void sendPackage(uint8_t *data, const bool notify);

  void sendPackage(char cmd, char mod, uint8_t value);

  void sendPackage(char cmd, char mod, uint8_t value, const bool notify);

  //static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

  void dbPingbackReceived();

  static NetworkController* instance();

private:
  static NetworkController* mInstance;

  EthernetUDP* dataServer;
  uint64_t last_dhcp = 0;

  //static uint64_t lastDBPing;

  uint64_t dbPingTrigger = 0;

  //static EthernetClient activeClient;

  //static WebSocketsServer* wsServer;

  //bool checkUTF8(uint8_t *string, int length);

  //static void refreshData();

};

#endif    // NETWORKCONTROLLER_H
