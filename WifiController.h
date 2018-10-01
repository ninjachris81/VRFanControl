#ifndef WIFICONTROLLER_H
#define WIFICONTROLLER_H

#include <AbstractTask.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "CommHandler.h"

#define AP_WIFI_NAME "FAWControl"
#define AP_VISIBILITY 0   // 0 = visible, 1 = hidden

#define WEBSERVER_DNS_NAME "fawcontrol"

//#define ENABLE_BASIC_AUTH

#define DNS_PORT 53

class WifiController : public AbstractTask, public CommHandler {
public:
  WifiController();

  void init();

  void update();

  void updateDataServer();

  static void onWebserverCss();
  static void onWebserverStatusPage();
  static void onWebserverNotFound();

  static WifiController* instance();
  
private:
  static WifiController* mInstance;
  
  static ESP8266WebServer* webServer;
  static WiFiServer* dataServer;

  static DNSServer dnsServer;

  static WiFiClient activeClient;
  
};


#endif    //WIFICONTROLLER_H
