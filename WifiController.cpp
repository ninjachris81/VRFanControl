#include "WifiController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "Credentials.h"
#include "Protocol.h"
#include "ServoController.h"

WifiController* WifiController::mInstance;

ESP8266WebServer* WifiController::webServer;
WiFiServer* WifiController::dataServer;

WiFiClient WifiController::activeClient;

WifiController::WifiController() : AbstractTask() {
  mInstance = this;
}

WifiController* WifiController::instance() {
  return mInstance;
}

void WifiController::init() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("FAW_Control", WIFI_PASSWORD, 1, 1);    // channel 1 and hidden

  webServer = new ESP8266WebServer(80);
  dataServer = new WiFiServer(8080);

  webServer->on("/", &WifiController::onWebserverStatusPage);
  webServer->onNotFound(&WifiController::onWebserverNotFound);
  webServer->begin();

  dataServer->begin();
}

void WifiController::update() {
  updateDataServer();
  webServer->handleClient();
}

void WifiController::updateDataServer() {
  if (activeClient) {
    if (activeClient.connected()) {
      if (activeClient.available() >= DATA_PACKAGE_SIZE) {
        uint8_t data[3];
        activeClient.read(data, DATA_PACKAGE_SIZE);
        //LOG_PRINTLN(data);
        handlePackage(taskManager, data);
      }
    }
  } else {
    activeClient = dataServer->available();
  }
}

void WifiController::onWebserverStatusPage() {
  String s = F("<!DOCTYPE html><html><head><title>Demo Status</title><body>");

  // TODO: add content
  if (activeClient && activeClient.connected()) {
    s+=F("Client connected: ");
    s+=activeClient.remoteIP().toString();
  } else {
    s+=F("No Client connected");
  }
  s+=F("<br>Servo States:<table>");

  for (uint8_t i=0;i<SERVO_COUNT;i++) {
    s+=F("<tr><td>Servo ");
    s+=i;
    s+=F(": ");
    s+=instance()->getTaskManager()->getTask<ServoController*>(SERVO_CONTROLLER)->getServoAngle(i);
    s+=F("&deg;</tr></td><br>");
  }
  s+=F("</table><br>");

  s+=F("Uptime: ");
  s+=millis()/1000;
  s+=F(" sec<br>");

  s += F("</body></html>");
  
  webServer->send(200, "text/html", s);
}

void WifiController::onWebserverNotFound() {
  webServer->send(404, "text/plain", "404: Not found");
}

