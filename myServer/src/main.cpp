#define BLINKER_WIFI
#include <Arduino.h>
#include <Blinker.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <String.h>

// String myhtmlPage =
//     String("") +
//     "<html>" +
//     "<head>" +
//     "    <title>ESP8266 Web Server Test</title>" +
//     "    <script defer=\"defer\">" +
//     "        function ledSwitch() {" +
//     "            var xmlhttp;" +
//     "            if (window.XMLHttpRequest) {" +
//     "                xmlhttp = new XMLHttpRequest();" +
//     "            } else {" +
//     "                xmlhttp = new ActiveXObject(\"Microsoft.XMLHTTP\");" +
//     "            }" +
//     "            xmlhttp.onreadystatechange = function () {" +
//     "                if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {" +
//     "                    document.getElementById(\"txtState\").innerHTML = xmlhttp.responseText;" +
//     "                }" +
//     "            }," +
//     "            xmlhttp.open(\"GET\", \"Switch\", true);" +
//     "            xmlhttp.send(); " +
//     "        }" +
//     "    </script>" +
//     "</head>" +
//     "<body>" +
//     "    <div id=\"txtState\">Unkwon</div>" +
//     "    <input type=\"button\" value=\"Switch\" οnclick=\"ledSwitch()\">" +
//     "</body>" +
//     "</html>";


char auth[] = "36031046b753";
char ssid[] = "小米路由器";
char pswd[] = "a.13513710972";

ESP8266WebServer myServer(80);
File fsUploadFile;
BlinkerButton Button1("btn-abc");
BlinkerButton Button2("btn-cde");
BlinkerText Text1("tex-abc");

String myhtml = String("")+
"<html>"+
"<head>"+
"    <title>ESP8266 Web Server Test</title>"+
"    <script defer=\"defer\">"+
"        function ledSwitch() {"+
"            var xmlhttp;"+
"            if (window.XMLHttpRequest) {"+
"                xmlhttp = new XMLHttpRequest();"+
"            } else {"+
"                xmlhttp = new ActiveXObject(\"Microsoft.XMLHTTP\");"+
"            }"+
"            xmlhttp.onreadystatechange = function () {"+
"                    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {"+
"                        document.getElementById(\"txtState\").innerHTML = xmlhttp.responseText;"+
"                    }"+
"                },"+
"            xmlhttp.open(\"GET\", \"Switch\", true);"+
"            xmlhttp.send(); "+
"        }"+
"    </script>"+
"</head>"+
"<body>"+
"    <div id=\"txtState\">Unkwon</div>"+
"    <input type=\"button\" value=\"Switch\" onclick=\"ledSwitch()\">"+
"</body>"+
"</html>";


void botton1_callback(const String & state){
    myServer.begin();
    Text1.print("网络服务器已打开");
    Serial.println("网络服务器已打开");
}
void botton2_callback(const String & state){
    myServer.close();
    Text1.print("网络服务器已关闭");
    Serial.println("网络服务器已关闭");
}
void homepage(){

    myServer.send(200, "text/html", myhtml);
    Serial.println("用户访问了主页");
}
void firstpage(){
    myServer.send(200,"text/html","这是第一个页面,不能显示中文吗");
    Serial.println("用户访问了第一个页面");
}
void secondpage(){
    myServer.send(200,"text/html","这是第二个页面");
    Serial.println("用户访问了第二个页面");
}

void setup() {
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    Blinker.begin(auth, ssid, pswd);
    Button1.attach(botton1_callback);
    Button2.attach(botton2_callback);
    myServer.on("/", homepage);
    myServer.on("/1", firstpage);
    myServer.on("/2", secondpage);
    SPIFFS.begin(); //开启文件系统
    Serial.println("文件系统开始");
}

void loop() {
    Blinker.run();
    myServer.handleClient();  //监听用户请求
}
