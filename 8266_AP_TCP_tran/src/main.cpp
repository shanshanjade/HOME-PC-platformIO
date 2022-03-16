/**
 * Demo：
 *    演示小程序UDP配网，需要结合配网小程序
 * @author 单片机菜鸟
 * @date 2021/01/02
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// char *ssid;     //wifi名称
// char *password = ""; //wifi密码

WiFiUDP Udp;
unsigned int localUdpPort = 8266; // 本地端口号
char incomingPacket[255];         // 接收缓冲区

#define AP_SSID "UDP_8266" //这里改成你的AP名字
#define AP_PSW  "12345678" //这里改成你的AP密码 8位以上
//以下三个定义为调试定义
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)
 
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

void setup()
{
  //以下为基本功能初始化，初始化串口和网络和LED
  pinMode(LED_BUILTIN, OUTPUT);
  //设置串口波特率，以便打印信息
  DebugBegin(115200);
  //延时2s 为了演示效果
  delay(2000);
  DebugPrint("Setting soft-AP configuration ... ");
  //配置AP信息
  WiFi.mode(WIFI_AP_STA);
  DebugPrintln(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  //启动AP模式，并设置账号和密码
  DebugPrint("Setting soft-AP ... ");
  boolean result = WiFi.softAP(AP_SSID, AP_PSW);
  if(result){
    DebugPrintln("Ready");
    //输出 soft-ap ip地址
    DebugPrintln(String("Soft-AP IP address = ") + WiFi.softAPIP().toString());
    //输出 soft-ap mac地址
    DebugPrintln(String("MAC address = ") + WiFi.softAPmacAddress().c_str());
  }else{
    DebugPrintln("Failed!");
  }
  DebugPrintln("Setup End");
  //以下开启UDP监听并打印输出信息
  Udp.begin(localUdpPort);
  Serial.printf("Now listening UDP port %d\n", localUdpPort);
}

void loop()
{
  int packetSize = Udp.parsePacket(); //获取当前队首数据包长度
  if (packetSize)  // 有数据可用
  {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255); // 读取数据到incomingPacket
    if (len > 0)                             // 如果正确读取
    {
      sendCallBack1("{\"code\":0}"); //设备成功收到信息
      delay(2000);
      incomingPacket[len] = 0; //末尾补0结束字符串
      Serial.printf("UDP packet contents: %s\n", incomingPacket);
      DebugPrintln(F("parseMqttResponse"));
      StaticJsonBuffer<100> jsonBuffer;
      // StaticJsonBuffer 在栈区分配内存   它也可以被 DynamicJsonBuffer（内存在堆区分配） 代替
      // DynamicJsonBuffer  jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(incomingPacket);
      if(!root.success()){
        DebugPrintln(F("parse failed"));
        return;
      }else{
        #ifdef DEBUG
          // 格式化打印json
          root.prettyPrintTo(Serial);
        #endif
        DebugPrintln(F("parse success"));
        sendCallBack1("{\"code\":1}");
        delay(500);
        String ssid = root["ssid"];
        unsigned int port = root["port"];
        String pwd = root["password"];
        //启动STA模式，并连接到wifi网络
        DebugPrint(F("ssid:"));
        DebugPrintln(ssid);
        DebugPrint(F("pwd:"));
        DebugPrintln(pwd);
        WiFi.begin(ssid, pwd);
        DebugPrint(String("Connecting to ")+ssid);
        //判断网络状态是否连接上，没连接上就延时1s，并且打出一个点，模拟连接过程
        unsigned int retry = 0;
        while (WiFi.status() != WL_CONNECTED){
          delay(1000);
          DebugPrint(".");
          retry++;
          if (retry == 60) {
          // basically die and wait for WDT to reset me
           sendCallBack1("{\"code\":3}");
           while(1);
          }
          ESP.wdtFeed();
          yield();
        }
        sendCallBack1("{\"code\":2}");
        DebugPrintln("");
        DebugPrint("Connected, IP address: ");
        //输出station IP地址，这里的IP地址由DHCP分配
        DebugPrintln(WiFi.localIP());
      }
    }
  }
}

/**
 * 发送响应信息
 */
void sendCallBack1(const char *buffer){
   DebugPrint(F("send rsp:"));
   DebugPrintln(buffer);
   Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
   Udp.write(buffer); //回复内容
   Udp.endPacket(); 
}




