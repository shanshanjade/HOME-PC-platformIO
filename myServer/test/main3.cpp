#define BLINKER_WIFI
#include <Arduino.h>
#include <Blinker.h>
#include <ESP8266WebServer.h>
#include <FS.h>
char auth[] = "36031046b753";
char ssid[] = "小米路由器";
char pswd[] = "a.13513710972";

ESP8266WebServer myServer(80);
File fsUploadFile;
BlinkerButton Button1("btn-abc");
BlinkerButton Button2("btn-cde");
BlinkerText Text1("tex-abc");

void botton1_callback(){
  myServer.begin();
  Text1.print("网络服务器已打开");
  Serial.println("网络服务器已打开");
}
void botton2_callback(){
  myServer.close();
  Text1.print("网络服务器已关闭");
  Serial.println("网络服务器已关闭");
}

void setup() {
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  myServer.on("/", HTTP_POST, respondOK, handleUserRequest);
  myServer.onNotFound(handleNotFound);
  SPIFFS.begin();
}

void loop() {
  Blinker.run();
  myServer.handleClient();
}

void handleFileUpload(){
  HTTPUpload& upload = myServer.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/" + filename;
    Serial.println("File name:" + filename);
    fsUploadFile = SPIFFS.open(filename, "w");

  } else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
    {
      fsUploadFile.write(upload.buf, upload.currentSize);
      Serial.println("向SPIFFS文件写入浏览器发来的文件数据");
    }
    
  }else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
    {
      fsUploadFile.close();
      Serial.println("文件大小:" + upload.totalSize);
      myServer.send(200, "text/plain", "文件上传成功");
      myServer.send(303);
    }else
    {
      Serial.println("File uploa failed");
      myServer.send(500, "text/plain", "500:couldn't create file");
    }
  }
}


void handleNotFound(){
  myServer.send(404,  "text/plain", "hello from esp8266");
}
void respondOK(){
  myServer.send(200);
}

void handleUserRequest(){

}