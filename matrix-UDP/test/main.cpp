#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

IPAddress localIP(192,168,23,1);
IPAddress gateway(192,168,23,1);
IPAddress subnet(255,255,255,0);

WiFiUDP udp;
char incomingPacket[255];
uint8_t incomingimg[1024];
// void sendCallBack(String buffer){
//    Serial.println(F("send rsp:"));
//    Serial.println(buffer);
//    udp.beginPacket(udp.remoteIP(), udp.remotePort());
//    udp.write(buffer); //回复内容
//    udp.endPacket(); 
// }
char  buffer[] = "{\"code\":0}";
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(localIP,gateway,subnet);
  WiFi.softAP("UDP_test","1234567890");
  Serial.println("AP done");
  udp.begin(8266);
  Serial.println("udp start");
  bool isConfigWifi = true;
  while (isConfigWifi)
  {
    
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      int len = udp.read(incomingimg,packetSize);
      if (len > 0 )
      {
        incomingPacket[len] = 0; //末尾补0结束字符串
        StaticJsonBuffer<100> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(incomingPacket);
        if(!root.success()){
          Serial.println("通过AP发送信息");
          Serial.println(incomingimg[0]);
        }else{
          // sendCallBack(F("{\"code\":1}"));
          String ssid = root["ssid"];
          String psw = root["psw"];
          Serial.println("获取到wifi名称和密码");
          WiFi.begin(ssid,psw);
          Serial.println("开始连接wifi");
          isConfigWifi = false;
          unsigned int retry = 0;
          while (WiFi.status() != WL_CONNECTED){
            delay(1000);
            Serial.println(".");
            retry++;
            if (retry == 60) {
              // sendCallBack(F("{\"code\":3}"));
              // basically die and wait for WDT to reset me
              while(1);
            }
            ESP.wdtFeed();
            yield();
          }
          // sendCallBack(F("{\"code\":2}"));
          Serial.println("已连接上wifi");
          Serial.println(WiFi.localIP().toString());
        }
      }
    }
    
  }
  
}

void loop(){
  int packetSize = udp.parsePacket();
  if(packetSize){
    int len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.println("通过局域网发送信息");
    Serial.println(incomingPacket);
  }

}

