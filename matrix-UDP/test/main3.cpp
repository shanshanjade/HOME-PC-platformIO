#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define PIN D3
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Ticker.h>

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
const uint16_t colors[] = { matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };
// IPAddress local_IP(192,168,4,1);
// IPAddress gateway(192,168,4,1);
// IPAddress subnet(255,255,255,0);
// #define AP_SSID "UDP_8266" //这里改成你的AP名字
// #define AP_PSW  "12345678" //这里改成你的AP密码 8位以上

WiFiUDP Udp;
unsigned int localUdpPort = 8266;  // 本地监听端口
uint8_t incomingPacket[1024];  // 存储Udp客户端发过来的数据
uint16_t imageArray[256];
uint8_t bitmap[] PROGMEM= {B1111,B0111,B1101,B1110};
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  // WiFi.softAPConfig(local_IP,gateway,subnet);
  // WiFi.softAP(AP_SSID,AP_PSW);
  // WiFi.begin("xiaomi123","a.13513710972");
  WiFi.begin("HUAWEIMATE30","12345678");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println("wifi已连接");
  Serial.println(WiFi.localIP().toString());
  Udp.begin(localUdpPort);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);

}

int x    = matrix.width();
int pass = 0;

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // 收到Udp数据包
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    // 读取Udp数据包
    int len = Udp.read(incomingPacket, 1024);
    if (len > 0)
    {
      incomingPacket[len] = 0;
      uint16_t pixValue = incomingPacket[0] | incomingPacket[1] << 8;
      Serial.println(incomingPacket[0]);
      Serial.println(incomingPacket[1]);
      Serial.println(uint16(incomingPacket[0]));
      Serial.println(uint16(incomingPacket[1])<<8);
      Serial.println(pixValue);
    }
    for (int i = 0; i < 1024; i=i+4)
    {
      imageArray[i/4] = matrix.Color(incomingPacket[i],incomingPacket[i+1],incomingPacket[i+2]);
    }
  }
  matrix.drawBitmap(0,0,bitmap,3,5,matrix.Color(255,255,0));
  // matrix.drawRGBBitmap(0,0,imageArray,32,8);
  matrix.show();
  delay(60);
  yield();
}