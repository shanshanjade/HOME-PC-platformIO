#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(80);
IPAddress softLocal(192,168,1,111);
IPAddress softGateway(192,168,1,111);
IPAddress softSubnet(255,255,255,0);


String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

void handleRoot(){
  String page = "this is form esp8266server!";
  server.sendHeader("Content-Length", String(page.length()));
  server.send(200,"text/plain",page);
}
void handleNotFound(){
  // server.send(404,"text/plain","404 not found");
  server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
  server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
}


void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(softLocal,softGateway,softSubnet);
    WiFi.softAP("ESP8266mDNS");
    IPAddress myIP = WiFi.softAPIP(); 
    Serial.println(myIP);
    Serial.println(WiFi.localIP());
    
    server.on("/",handleRoot);
    server.onNotFound(handleNotFound);
    MDNS.begin("esp8266",myIP);
    server.begin();
    MDNS.addService("http", "tcp", 80);
}

void loop(){
    MDNS.update();
    server.handleClient();
}