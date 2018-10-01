#include "WifiController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "Credentials.h"
#include "Protocol.h"
#include "ServoController.h"
#include "FanController.h"

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
  WiFi.softAP(AP_WIFI_NAME, WIFI_PASSWORD, 1, 1);    // channel 1 and hidden

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
  String s = F("<!DOCTYPE html><html><head><META HTTP-EQUIV=\"refresh\" content=\"10\"><title>Demo Status</title><body>");

#ifdef ENABLE_BASIC_AUTH
  if (!webServer->authenticate(BASICAUTH_USERNAME, BASICAUTH_PASSWORD)) {
    return webServer->requestAuthentication();
  }
#endif
  
  // TODO: add content
  if (activeClient && activeClient.connected()) {
    s+=F("Client connected: ");
    s+=activeClient.remoteIP().toString();
    s+="<br>";
  } else {
    s+=F("No Client connected<br>");
  }

  // uptime
  s+=F("Uptime: ");
  s+=(int)(millis()/1000);
  s+=F(" sec<br>");

  // S/N rate
  s+=F("S/N: ");
  s+=WiFi.RSSI();
  s+=F("<br>");

  // Servos
  s+=F("Servo States:<table>");
  for (uint8_t i=0;i<SERVO_COUNT;i++) {
    s+=F("<tr><td>Servo ");
    s+=i;
    s+=F(": ");
    s+=instance()->getTaskManager()->getTask<ServoController*>(SERVO_CONTROLLER)->getServoAngle(i);
    s+=F("&deg;</tr></td><br>");
  }
  s+=F("</table><br>");

  // Fans
  s+=F("<br>Servo States:<table>");
  for (uint8_t i=0;i<SPEED_COUNT;i++) {
    s+=F("<tr><td>Fan ");
    s+=i;
    s+=F(": ");
    s+=instance()->getTaskManager()->getTask<FanController*>(FAN_CONTROLLER)->getSpeedLevel((FanController::SPEED_LOCATION)i);
    s+=F("</tr></td><br>");
  }
  s+=F("</table><br>");

  s += F("</body></html>");
  
  webServer->send(200, "text/html", s);
}

void WifiController::onWebserverNotFound() {
  webServer->send(404, "text/plain", "404: Not found");
}

