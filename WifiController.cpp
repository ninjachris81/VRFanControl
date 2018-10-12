#include "WifiController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "Credentials.h"
#include "Protocol.h"
#include "ServoController.h"
#include "FanController.h"

WifiController* WifiController::mInstance;
DNSServer WifiController::dnsServer;

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
  LOG_PRINTLN(F("Setting AP"));
  
  if (WiFi.softAP(AP_WIFI_NAME, WIFI_PASSWORD, 1, AP_VISIBILITY)) {
    LOG_PRINT(F("AP IP: "));
    LOG_PRINTLN(WiFi.softAPIP().toString());

    dnsServer.setTTL(300);
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNS_PORT, WEBSERVER_DNS_NAME, WiFi.softAPIP());
    
//    WiFi.config(WiFi.softAPIP(), WEBSERVER_DNS_NAME);
  } else {
    LOG_PRINTLN(F("Failed to set AP"));
  }

  LOG_PRINTLN(F("Init web server"));
  webServer = new ESP8266WebServer(80);
  webServer->on("/", &WifiController::onWebserverStatusPage);
  webServer->on("/style.css", &WifiController::onWebserverCss);
  webServer->onNotFound(&WifiController::onWebserverNotFound);
  webServer->begin();

  LOG_PRINTLN(F("Init data server"));
  dataServer = new WiFiServer(8080);
  dataServer->begin();
}

void WifiController::update() {
  updateDataServer();
  webServer->handleClient();
  dnsServer.processNextRequest();
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
  String s = F("<!DOCTYPE html><html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"><META HTTP-EQUIV=\"refresh\" content=\"10\"><title>Demo Status</title><body>");

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

  // ip
  s+=F("IP: ");
  s+=WiFi.softAPIP().toString();
  s+=F("<br>");
  
  // S/N rate
  s+=F("S/N: ");
  s+=WiFi.RSSI();
  s+=F("<br>");

  // Servos
  /*
  s+=F("<table><th>Servo States</th>");
  for (uint8_t i=0;i<SERVO_COUNT;i++) {
    s+=F("<tr><td>Servo ");
    s+=i;
    s+=F(": ");
    s+=instance()->getTaskManager()->getTask<ServoController*>(SERVO_CONTROLLER)->getServoAngle(i);
    s+=F("&deg;</tr></td><br>");
  }
  s+=F("</table><br>");
  */

  // Fans
  s+=F("<table><th>Servo States</th>");
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

void WifiController::onWebserverCss() {
  String s = F("td{border:1px solid #dddddd;}th{background-color: #dddddd;}");
  webServer->send(200, "text/css", s);
}

void WifiController::onWebserverNotFound() {
  webServer->send(404, "text/plain", "404: Not found");
}
