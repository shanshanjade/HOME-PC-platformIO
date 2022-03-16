#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif
#define PIN D3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
const uint16_t colors[] = { matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

#define AP_SSID "UDP_8266" //这里改成你的AP名字
#define AP_PSW  "12345678" //这里改成你的AP密码 8位以上

WiFiUDP Udp;
unsigned int localUdpPort = 8266;  // 本地监听端口
char incomingPacket[255];  // 存储Udp客户端发过来的数据

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP,gateway,subnet);
  WiFi.softAP(AP_SSID,AP_PSW);
    //启动Udp监听服务
  Udp.begin(localUdpPort);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);

}

int x    = matrix.width();
int pass = 0;
String lcdprint = "Wait.....";


void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // 收到Udp数据包
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    // 读取Udp数据包
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    //向串口调试器打印信息
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    lcdprint = incomingPacket;
  }

  matrix.fillScreen(0);
  // matrix.setCursor(0,0);
  // matrix.drawRGBBitmap(0,0,bitmap24,24,24);
  matrix.setCursor(x, 0);
  matrix.print(lcdprint);
  int a = -lcdprint.length()*6;
  if(--x < a) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(60);
  yield();
}