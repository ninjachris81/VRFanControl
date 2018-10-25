#ifndef WIFICONTROLLER_H
#define WIFICONTROLLER_H

#include <AbstractTask.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include "CommHandler.h"

#define AP_WIFI_NAME "FAWControl"
#define AP_CHANNEL 1
#define AP_VISIBILITY 0   // 0 = visible, 1 = hidden

#define LEVEL_COUNT 3

#define WEBSERVER_DNS_NAME "fawcontrol"

//#define ENABLE_BASIC_AUTH

#define DNS_PORT 53

#define DB_PING_INTERVAL 4000

class WifiController : public AbstractTask, public CommHandler {
public:
  WifiController();

  void init();

  void update();

  bool useInitWDT();

  void sendPackage(uint8_t *data);
  void sendPackage(uint8_t *data, bool notify);

  void sendPackage(char cmd, char mod, uint8_t value);
  void sendPackage(char cmd, char mod, uint8_t value, bool notify);
  
  void updateDataServer();

  void notifyPackage(uint8_t *data);

  void dbPingbackReceived();

  static void onWebserverCss();
  static void onWebserverJs();
  static void onWebserverStatusPage();
  static void onWebserverNotFound();

  static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

  static WifiController* instance();
  
  uint64_t dbPingTrigger = 0;
  
private:
  static uint64_t lastDBPing;

  static WifiController* mInstance;
  
  static ESP8266WebServer* webServer;
  static WiFiServer* dataServer;
  static WebSocketsServer* wsServer;

  static DNSServer dnsServer;

  static WiFiClient activeClient;

  static void refreshData();
  
};


#endif    //WIFICONTROLLER_H
