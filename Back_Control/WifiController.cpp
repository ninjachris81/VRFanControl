#include "WifiController.h"

#include <LogHelper.h>
#include "TaskIDs.h"
#include "Credentials.h"
#include "Protocol.h"
#include "CommController.h"
#include "GlobalConstants.h"
#include "Debug.h"

#include <ArduinoJson.h>

WifiController* WifiController::mInstance;
DNSServer WifiController::dnsServer;

ESP8266WebServer* WifiController::webServer;
WiFiServer* WifiController::dataServer;
WebSocketsServer* WifiController::wsServer;

WiFiClient WifiController::activeClient;

uint64_t WifiController::lastDBPing = 0;

WifiController::WifiController() : AbstractTask() {
  mInstance = this;
}

WifiController* WifiController::instance() {
  return mInstance;
}

bool WifiController::useInitWDT() {
  return false;
}

void WifiController::init() {
  LOG_PRINTLN(F("Setting AP"));

  WiFi.disconnect();

  if (WIFI_CLIENT_MODE) {
    WiFi.hostname(WEBSERVER_DNS_NAME);
    WiFi.begin(WIFI_CLIENT_SSID, WIFI_CLIENT_PASSWORD);
  } else {
    if (WiFi.softAP(AP_WIFI_NAME, WIFI_AP_PASSWORD, AP_CHANNEL, AP_VISIBILITY)) {
      LOG_PRINT(F("AP IP: "));
      LOG_PRINTLN(WiFi.softAPIP().toString());
  
      dnsServer.setTTL(300);
      dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
      dnsServer.start(DNS_PORT, WEBSERVER_DNS_NAME, WiFi.softAPIP());
      
  //    WiFi.config(WiFi.softAPIP(), WEBSERVER_DNS_NAME);
    } else {
      LOG_PRINTLN(F("Failed to set AP"));
    }
  }

  LOG_PRINTLN(F("Init ws server"));
  wsServer = new WebSocketsServer(81);
  wsServer->begin();
  wsServer->onEvent(&WifiController::onWebSocketEvent);

  LOG_PRINTLN(F("Init web server"));
  webServer = new ESP8266WebServer(80);
  webServer->on("/", &WifiController::onWebserverStatusPage);
  webServer->on("/style.css", &WifiController::onWebserverCss);
  webServer->on("/script.js", &WifiController::onWebserverJs);
  
  webServer->onNotFound(&WifiController::onWebserverNotFound);
  webServer->begin();

  LOG_PRINTLN(F("Init data server"));
  dataServer = new WiFiServer(8080);
  dataServer->begin();
}

void WifiController::update() {
  int newStatus = WiFi.status();
  if (lastWifiStatus!=newStatus) {
    LOG_PRINT(F("Wifi status changed to: "));
    LOG_PRINTLN(newStatus==WL_CONNECTED ? "Connected" : newStatus==WL_DISCONNECTED ? "Disconnected" : String(newStatus));
    if (newStatus==WL_CONNECTED) {
      LOG_PRINT(F("Wifi IP: "));
      LOG_PRINTLN(WiFi.localIP());
    }
  }
  lastWifiStatus = newStatus;
  
  updateDataServer();
  wsServer->loop();
  webServer->handleClient();
  dnsServer.processNextRequest();

  if (millis() - dbPingTrigger > DB_PING_INTERVAL) {
    dbPingTrigger = millis();
    instance()->getTaskManager()->getTask<CommController*>(COMM_CONTROLLER)->sendPackage(CMD_PING, MOD_NONE, 0);
  }
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

void WifiController::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

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

void WifiController::refreshData() {
  StaticJsonBuffer<512> jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();

  // active client
  if (activeClient && activeClient.connected()) {
    array.add(activeClient.remoteIP().toString());
  } else {
    array.add("-");
  }

  // ap clients
  array.add(WiFi.softAPgetStationNum());

  // ws clients
  array.add(wsServer->connectedClients());

  // uptime
  array.add((int)(millis()/1000));

  // ap ip
  array.add(WiFi.softAPIP().toString());

  // sn rate
  array.add(WiFi.RSSI());

  // db pingback
  array.add((int)(lastDBPing/1000));

  // free heap
  array.add(ESP.getFreeHeap());

  String buf;
  array.printTo(buf);  
  wsServer->broadcastTXT(buf);
}

void WifiController::notifyPackage(uint8_t *data) {
  LOG_PRINTLN(F("Notify package"));
  if (checkUTF8(data, DATA_PACKAGE_SIZE)) {
    wsServer->broadcastTXT(data, DATA_PACKAGE_SIZE);
  } else {
    LOG_PRINTLN(F("Invalid UTF-8"));
  }
}

void WifiController::dbPingbackReceived() {
  if (PRINT_PING) {
    LOG_PRINTLN(F("DB ping received"));
  }

  lastDBPing = millis();
}

void WifiController::onWebserverStatusPage() {  
  String s = F("<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"><link rel=\"icon\" href=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAJLElEQVRYR51Xe0xTaRb/bl+0lQKlCC1SBIpWeRgIBlAQNmQiq5EIWdEdVjcxzJpYUWJdoZTqVgQsnbHqgLphQzKZTdBViVHIGs0sWR8g+4coSHhDgcICpYXyLPRx7+Z8M3RAhZ3xJE16b3vP+Z3f+Z1zvkugzzSNRqMjCOIrhJA7RVHXFQqF/HNcEb/kIa1Wm0mS5KhCoXgF/y8rKwvg8XidJ06c2ADXlZWV87Ozs9vy8/OH4Vqj0SQSBBHFZDKr5HK5db0Y6wIoKyv7A0VRX4WFhcXPzs6SQ0NDXxcUFKg1Gk3a1q1b/5GWlsYB57W1tdbu7u4jCoWiVqPRKMVicaFQKKQ3NzfbSJJU5ufnV6wFYk0AGo0m3MfH5z+pqalscGaz2ajq6uoFk8mkIAjCIyEh4S/x8fEscNzU1GRraGi4RFHUDJ/P12RlZXE5HA4xPT1NPnnyZNFgMHyhVCpffwrEmgBKSkpSt2zZ8jAjIwNnCWaxWLDDwMBAenh4ONPLy4u2fL+jo8NuMBice/fuZS/fh9/q6uqsnZ2dxwoKCmp+FQC1Ws1is9kmmUzmzmazf5FWPgzgcDhQeXn5LJPJ9FtLC6scX7lyJcbpdPo7HI6narXaVlJScnv//v3ZkO2y86WlJWQymdDMzAxaWFhATqcTubu7I09PTyQQCBCbzXbh6OzstNfW1v69sLAwW6fTcRYWFr5ACE2pVCosZjAXgJKSkt+LRKIqHo9H9Pb2OpxO510ej/floUOHuD4+PjQIbDQa0eTkJPLx8cEBuVwuYjAYaHZ2Fk1PTyOz2YzB+Pr6og0bNuCSPXjwwDo1NXWfwWD8TiKRMKxWKzU0NJSvUqmwMF0AiouLfzh69OhvRCIRFhyA8PX1pUPwiYkJNDg4iAICArBzADE1NYUWFxeR1WpFXl5eiM/nYwYABPw3MDAQ/xdAjI+PkyEhIXQmk0nAdVVV1bzNZgtQq9UzGMClS5cyw8PDvz9w4MDP/P1EEdA9NDSEpFIppru/vx95eHjggG5ubpgFi8WCAcEnODgYl6GrqwuJRCLk5+f3kfZevHix1NTUpLx48aIOAygqKooRCoUv0tPTOVwu18WKzWZDra2tKCwsDGcKwUNDQ3HwTxmUAdiCMnh7e6O2tjYMHMqxQkPU48ePrYODg6lqtfqVK5harVZ4enqqoO08PDxwew0PD+MswWF7ezvavn07zng9A9DwHOiEJEnMTlBQEH5kfn6efPTokdVoNJYVFRVdXqUBuLhw4UKOWCwuS0tL49rtdtTd3Y2io6MxC0Dlysy7urrsPT09drPZzI2Pj7dLpVJXp8zNzWEdREVFYRZADxwOBz19+tTa19enunz5sm5VFyiVSj8ajRaJEIqOi4srjoyMZAGVYDweD42Pj6PNmzfja4qiUH19PVD4V4qi7lMUxaPRaL+VSCQnk5KS2ATxI6kjIyO4U2AWQPlAD9CWjY2NpSRJviJJ8n1paek4oVKpdO7u7n/y8/NjCAQCmlgsZnA4HNrAwACmDoKDqMAZmF6vtzc0NHxbXFz855WlUKlU3yQnJ58Ri8WYCWBhmX4QpEQiQdBd8Pzk5CR0hmNubu5vRGFh4czhw4d5H9a1r68PRUZGYjVv3LjRNWCePXu2YLFYwkpKSgZXPqNUKneIRKLXSUlJWCSQOSSxc+dO2BVo27ZtH0nn3r17s0RBQQGZkZHx0ajt7e1FsbGxqLm5GW3atAkxmT+WuLa2lrLb7Vs1Gk3vhwDc3Nxa9u3b5yoV+Ni9ezdqbGzEDNBoWNsue/jwIUXk5+frPD09T8DQ8fLyont7ezPodDoBNYSWGxsbww+CFsCMRqP9zZs338DaXelMoVCUxsbGnhcIBAy4D2MaPjAXoIPEYjF0BWUymRwWi8U5MTHhnJ6ersSZ5+Xl+VMUFUWj0XbExMQU+fv7M2GoQNYwdAAMiGjZ3r59azUYDLdIkvyeIAgaQRBHxWKxLDo62rU5QTvQijC8YG/AdwDf1NRUShDEa4qi3mu12v+uoj4vLy8nICBAGxcXx1lZw3fv3uHBAmCWbXR01N7f328fGxvjJiQk2IVCoasN5+fnMXMxMTGopaUFtzAIubm5eVGv15/XarWuA4oLgFwuv+Dj46NISUnhsFgsfB+cwBCCwO/fv0chISG4n9czWFqQvb+/PxYi7AbQEBh0AbSwyWTS6HS6nwfRuXPnEoOCgp7t2bOHzWAwVo3ijo4O3A3Qyz09PXgerDWKYSvC7oClBAsKsgcdrQTtcDgAxOLIyMieq1evvsHB5HK5fNeuXVeioqLwEWulweYzGAx4DIPBdIRxDCBYLBZ2DjUGzQAACAh0wwQUCoV4I35oL1++XGprazum0+nuYwCnT5/2YLFYw8ePH9/A4/FogHJgYMDh7e0NXUEDGmEuQPZALVxDpkA31BvAgMjgA/Tr9Xo8fqH2cJgdHR11BgUFMaC04+Pjzurq6n9dv349FWK76M7Nzc0JDg4ug8NkT0+Pw2633/Xw8PjyyJEjXJiQEGx0dBRvO3AMFAMTsKxgCwIDcGABMJA5/AaH0rt371onJye/o9Ppf5RKpQyz2UwajcbjN27cuL8KAFzk5uYmkiTJZzAYP1y7ds165syZ2+np6dkREREuhcO2gyxhzIIuANjy+QDoXnkka29vt9fU1HxXXl5+IjMzky4SiVJJkuyvqKjoXC7LmofNs2fPcgiCGD9//ry7m5vbZx1KQfVarXaOoig/SOhT3bOm45ycnPSIiIjqzMxMV99NTU3BGW8xNDSUvmPHDiaUBpwCrW1tbXa9Xu88ePAgm8/nu2ZuTU2NtbW19VhFRcWvO5bLZLJEiUTydNnh0tISVVlZuWA2mxUURbmnpKSok5OT3QDA8+fPl+rr69UEQcwJBAJNdnY2F05Wg4ODzrq6ukWj0bhzJe0rmViXWplMlsNgMEoTExNZw8PDzv7+/q8rKirUOTk5qVKp9GFWVhZm586dO9aOjo5DN2/e/OepU6fwqxmfzydaWlr+TaPRqtbK/iMRfqpG0KIOhwPegt/cvn37+U9ti19O5XI5PuzpdDr8clpeXo5fTk+ePBlDp9OD1gv8f0W47rxFCMlkMjhW4ddzhND1W7dufdbr+f8APLeLHuxZoBgAAAAASUVORK5CYII=\"><script src=\"script.js\"></script><title>Demo Status</title><body>");

if (ENABLE_BASIC_AUTH) {
  if (!webServer->authenticate(BASICAUTH_USERNAME, BASICAUTH_PASSWORD)) {
    return webServer->requestAuthentication();
  }
}
  
  s+=F("<table align=center><th>Item</th><th>Value</th>");

  // TODO: clients connected
  s+=F("<tr><td width=50%>Data client connected</td><td id='f0'></td></tr>");

  // ap clients
  s+=F("<tr><td>AP Clients</td><td id='f1'></td></tr>");

  // ws clients
  s+=F("<tr><td>WS Clients</td><td id='f2'></td></tr>");

  // uptime
  s+=F("<tr><td>Uptime</td><td id='f3'></td></tr>");

  // ap ip
  s+=F("<tr><td>IP</td><td id='f4'></td></tr>");
  
  // S/N rate
  s+=F("<tr><td>S/N</td><td id='f5'></td></tr>");

  // DB controller
  s+=F("<tr><td>DBC</td><td id='f6'></td></tr>");

  // Free heap
  s+=F("<tr><td>Heap</td><td id='f7'></td></tr>");

  // Fan
  s+=F("<tr><td>Fan State</td><td><div contenteditable=true id='eid_Fx'></div><input type=button onclick=\"sendData(this,'f');\" value='SET'></td></tr>");

  // Levels
  s+=F("<tr><td colspan=2><table width=100%><th colspan=2>Levels</th>");
  for (uint8_t i=0;i<LEVEL_COUNT;i++) {
    s+=F("<tr><td width=50%>Level ");
    s+=INDEX_TO_LOCATION_MOD(i);
    s+=F("</td><td><span id='eid_L");
    s+=INDEX_TO_LOCATION_MOD(i);
    s+=F("'></span></td></tr>");
  }
  s+=F("</table></td></tr>");

  // smells
  s+=F("<tr><td colspan=2><table width=100%><th colspan=3>Mist Makers</th>");
  for (uint8_t i=0;i<MM_COUNT;i++) {
    s+=F("<tr><td>Smell ");
    s+=i;
    s+=F("</td><td width=20%><span id='eid_M");
    s+=INDEX_TO_LOCATION_MOD(i);
    s+=F("'></span></td><td><input type=button onclick=\"sendCmd('v','");
    s+=INDEX_TO_LOCATION_MOD(i);
    s+=F("',5);\" value='EMIT'></td></tr>");
  }
  s+=F("</table></td></tr>");

  // seat
  s+=F("<tr><td colspan=2>");
  s+=F("<table width=100%><th colspan=2>Seat</th>");
  s+=F("<tr><td>Position</td><td><span id='eid_Sx'></span></td></tr>");
  s+=F("<tr><td>Movement</td><td><span id='eid_Ox'></span></td></tr>");
  s+=F("<tr><td>Forward Switch</td><td><span id='eid_Wf'></span></td></tr>");
  s+=F("<tr><td>Backward Switch</td><td><span id='eid_Wb'></span></td></tr>");
  s+=F("<tr><td><input type=button value='Move Forward' onclick=\"sendCmd('s','f',0)\"></td><td><input type=button value='Move Backward' onclick=\"sendCmd('s','b',0)\"></td></tr>");
  s+=F("</td></tr>");
  
  // colors
  s+=F("<tr><td colspan=2><table width=100%><th colspan=3>LED Colors</th>");
  for (uint8_t i=0;i<LED_FLOAT_STRIP_COUNT;i++) {
    s+=F("<tr><td>LED Color ");
    s+=i;
    s+=F("</td><td><div contenteditable=true id='eid_E");
    s+=FLOAT_INDEX_TO_LED_MOD(i);
    s+=F("'></div><input type=button onclick=\"sendData(this, 'e');\" value='SET'></td></tr>");
  }
  s+=F("</table></td></tr>");

  // brightnesses
  s+=F("<tr><td colspan=2><table width=100%><th colspan=2>LED Brightnesses</th>");
  for (uint8_t i=0;i<LED_FLOAT_STRIP_COUNT;i++) {
    s+=F("<tr><td>LED Brightness ");
    s+=i;
    s+=F("</td><td><div contenteditable=true id='eid_B");
    s+=FLOAT_INDEX_TO_LED_MOD(i);
    s+=F("'></div><input type=button onclick=\"sendData(this, 'b');\" value='SET'></td></tr>");
  }
  s+=F("</table></td></tr>");

  
  // finish main table
  s+=F("</table>");

  s += F("</body></html>");
  
  webServer->send(200, "text/html", s);
}

void WifiController::onWebserverCss() {
  String s = F("*{font-family:Arial;font-size:large;}td{border:1px solid #dddddd;}th{background-color: #dddddd;}div{display:inline-block;background-color:khaki;width:40px;}");
  webServer->send(200, "text/css", s);
}

void WifiController::onWebserverJs() {
  // https://www.danstools.com/javascript-minify/
  String s = F("var conn=new WebSocket('ws://'+location.hostname+':81/');setInterval(function(){if(conn.readyState==WebSocket.OPEN){conn.send('refresh');}},2000);function sendData(elem,cmd){if(conn.readyState==WebSocket.OPEN){conn.send('@'+cmd+elem.previousElementSibling.id.substr(5)+''+parseInt(elem.previousElementSibling.innerHTML));}}function sendCmd(cmd,mod,v){if(conn.readyState==WebSocket.OPEN){conn.send('@'+cmd+mod+v);}}conn.onerror=function(error){console.error('WS ',error);};conn.onmessage=function(e){if(e.data.length==4&&e.data.charAt(0)=='@'){var mod=e.data.charAt(2);var eid='eid_'+e.data.charAt(1,1)+mod;var value=e.data.charCodeAt(3,3)-48;if(document.getElementById(eid)){document.getElementById(eid).innerHTML=value;}else{console.error('Unknown eid: ',eid);}}else if(e.data.length>4&&e.data.charAt(0)=='['){var data=JSON.parse(e.data);for(var i=0;i<data.length;i++){document.getElementById('f'+i).innerHTML=data[i];}}else{console.error('WS ','invalid data: ',e.data);}};");
  webServer->send(200, "text/javascript", s);
}

void WifiController::onWebserverNotFound() {
  webServer->send(404, "text/plain", "404: Not found");
}

void WifiController::sendPackage(uint8_t *data) {
  sendPackage(data, false);
}

void WifiController::sendPackage(uint8_t *data, bool notify) {
  if (activeClient) {
    if (activeClient.connected()) {
      for (uint8_t i=0;i<DATA_PACKAGE_SIZE;i++) {
        activeClient.write(data[i]);
      }
      activeClient.println("");
      activeClient.flush();
    }
  }

  if (notify) {
    notifyPackage(data);
  }
}

void WifiController::sendPackage(char cmd, char mod, uint8_t value) {
  sendPackage(cmd, mod, value, false);
}

void WifiController::sendPackage(char cmd, char mod, uint8_t value, bool notify) {
  uint8_t package[DATA_PACKAGE_SIZE];

  package[0] = CMD_IDENTIFIER;
  package[1] = cmd;
  package[2] = mod;
  package[3] = value + '0';

  sendPackage(package, notify);
}

bool WifiController::checkUTF8(uint8_t *string, int length)
{
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
}
