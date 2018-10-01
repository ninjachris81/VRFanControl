#ifndef WIFICONTROLLER_H
#define WIFICONTROLLER_H

#include <AbstractTask.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "CommHandler.h"

class WifiController : public AbstractTask, public CommHandler {
public:
  WifiController();

  void init();

  void update();

  void updateDataServer();

  static void onWebserverStatusPage();
  static void onWebserverNotFound();

  static WifiController* instance();
  
private:
  static WifiController* mInstance;
  
  static ESP8266WebServer* webServer;
  static WiFiServer* dataServer;

  static WiFiClient activeClient;
  
};


#endif    //WIFICONTROLLER_H
