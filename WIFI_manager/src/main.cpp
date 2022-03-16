
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <CustomWiFiManager.h>     //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <Ticker.h>
#include <DoubleResetDetect.h>

void initSystem();
void initWifiManager();
void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void tick();

// const char blueWifi[] PROGMEM = "data:image/png;base64,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ZXguaHRtbL21eQoAAABjdEVYdHN2Zzpjb21tZW50ACBHZW5lcmF0b3I6IEFkb2JlIElsbHVzdHJhdG9yIDE5LjAuMCwgU1ZHIEV4cG9ydCBQbHVnLUluIC4gU1ZHIFZlcnNpb246IDYuMDAgQnVpbGQgMCkgIM5IkAsAAAAYdEVYdFRodW1iOjpEb2N1bWVudDo6UGFnZXMAMaf/uy8AAAAYdEVYdFRodW1iOjpJbWFnZTo6SGVpZ2h0ADQxMo5PObYAAAAXdEVYdFRodW1iOjpJbWFnZTo6V2lkdGgANTI03jL1KgAAABl0RVh0VGh1bWI6Ok1pbWV0eXBlAGltYWdlL3BuZz+yVk4AAAAXdEVYdFRodW1iOjpNVGltZQAxNTQ2OTc3NzgzgH8AMAAAABJ0RVh0VGh1bWI6OlNpemUAMjA4NDVC9X3U4QAAAGJ0RVh0VGh1bWI6OlVSSQBmaWxlOi8vL2hvbWUvd3d3cm9vdC9uZXdzaXRlL3d3dy5lYXN5aWNvbi5uZXQvY2RuLWltZy5lYXN5aWNvbi5jbi9maWxlcy8xMjAvMTIwNTg1OS5wbmctUab6AAAAAElFTkSuQmCC";

bool shouldSaveConfig = false;
Ticker ticker;
DoubleResetDetect drd(5.0,0x00);


char mqtt_server[40];
char mqtt_port[6] = "8080";
char api_key[34] = "Your ApiKey";




void initSystem(){
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);
}

/**
 * 功能描述：初始化wifimanager
 */
void initWifiManager(){
  WiFiManager wifiManager;
  // wifiManager.resetSettings();
  // wifiManager.setHeadImgBase64(FPSTR(blueWifi));
  wifiManager.setPageTitle("配置AWTRIX连接WIFI");
  wifiManager.setButtonBackground("#2394BC");
  wifiManager.setConnectTimeout(20);
  wifiManager.setDebugOutput(true);
  wifiManager.setMinimumSignalQuality(30);
  IPAddress _ip = IPAddress(192, 168, 4, 25);
  IPAddress _gw = IPAddress(192, 168, 4, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);
  wifiManager.setAPStaticIPConfig(_ip, _gw, _sn);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setBreakAfterConfig(true);
  wifiManager.setRemoveDuplicateAPs(true);

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_apikey("apikey", "onenet apikey", api_key, 32);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_apikey);

  if(drd.detect()){
    wifiManager.resetSettings();
    Serial.println("RESET!..............................................");
    ESP.reset();
  }

  if (!wifiManager.autoConnect("awtrix")) {
      Serial.println("failed to connect and hit timeout");
      ESP.reset();
      delay(1000);
  }

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(api_key, custom_apikey.getValue());
  Serial.println(mqtt_server);
  Serial.println(mqtt_port);
  Serial.println(api_key);

  if (shouldSaveConfig) {
      Serial.println("saving config");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["mqtt_server"] = mqtt_server;
      json["mqtt_port"] = mqtt_port;
      json["api_key"] = api_key;
      json.printTo(Serial);
  }
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
}

/**
 * 功能描述：配置进入AP模式通知回调
 */
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

/**
 * 功能描述：设置点击保存的回调
 */
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

/**
 * 功能描述：设置LED灯闪烁，提示用户进入配置模式
 */
void tick(){
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void setup() {
  // put your setup code here, to run once:
  initSystem();
  initWifiManager();
  //if you get here you have connected to the WiFi
  Serial.println("connected...so easy :)");
  ticker.detach();




  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
}