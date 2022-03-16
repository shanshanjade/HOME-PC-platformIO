#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <Ticker.h>

void initSystem();
void initWifiManager();
void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void tick();

bool shouldSavaeConfig = false;
Ticker ticker;

char mqtt_server[40];
char mqtt_port[6] = "8080";
char api_key[34] = "your apikey";



void initSystem(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  ticker.attach(0.6,tick);
}

void initWiFiManager(){
  WiFiManager wifimanager;
  wifimanager.resetSettings();
  wifimanager.setConnectTimeout(60);
  wifimanager.setDebugOutput(true);
  wifimanager.setMinimumSignalQuality(30);
  IPAddress _ip = IPAddress(192,168,4,25);
  IPAddress _gateway = IPAddress(192,168,4,1);
  IPAddress _subnet = IPAddress(255,255,255,0);
  wifimanager.setAPStaticIPConfig(_ip,_gateway,_subnet);
  wifimanager.setAPCallback(configModeCallback);
  wifimanager.setSaveConfigCallback(saveConfigCallback);
  wifimanager.setBreakAfterConfig(true);
  wifimanager.setRemoveDuplicateAPs(true);
  WiFiManagerParameter custom_mqtt_server("server","mqtt server",mqtt_server,40);
  WiFiManagerParameter custom_mqtt_port("port","mqtt port",mqtt_port,6);
  WiFiManagerParameter custom_mqtt_apikey("server","onenet apikey",api_key,32);
  wifimanager.addParameter(&custom_mqtt_server);
  wifimanager.addParameter(&custom_mqtt_port);
  wifimanager.addParameter(&custom_mqtt_apikey);

  if(!wifimanager.autoConnect("fefsef","121851513")){
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  if(shouldSavaeConfig){
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["api_key"] = api_key;
    json.printTo(Serial);
  }

  Serial.println("local ip:");
  Serial.println(WiFi.localIP());

}

void configModeCallback(WiFiManager *myWiFiManager){
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2,tick);

}

void saveConfigCallback(){
  Serial.println("Should save config");
  shouldSavaeConfig = true;
}

void tick(){
  int state = digitalRead(LED_BUILTIN);
  digitalWrite(LED_BUILTIN,!state);
}

void setup() {
  initSystem();
  initWifiManager();
  Serial.println("connected...so easy :)");
  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {

}