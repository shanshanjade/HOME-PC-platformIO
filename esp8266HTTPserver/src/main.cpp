#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <FS.h>
ESP8266WiFiMulti wifiMulti;
ESP8266WebServer esp8266_server(80);
String getContentType(String filename)
{
	if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}
bool handleFileRead(String path)
{ //澶勭悊娴忚�堝櫒HTTP璁块�?

	if (path.endsWith("/"))
	{						  // 濡傛灉璁块棶鍦板潃浠�?"/"涓虹粨灏�?
		path = "/index.html"; // 鍒欏皢璁块棶鍦板潃淇�鏀逛负/index.html渚夸簬SPIFFS璁块�?
	}

	String contentType = getContentType(path); // 鑾峰彇鏂囦�?�绫诲�?

	if (SPIFFS.exists(path))
	{												  // 濡傛灉璁块棶鐨勬枃浠跺彲浠ュ湪SPIFFS涓�鎵惧�?
		File file = SPIFFS.open(path, "r");			  // 鍒欏皾璇曟墦�?�?璇ユ枃浠�?
		esp8266_server.streamFile(file, contentType); // 骞朵笖灏嗚�ユ枃浠惰繑鍥炵粰娴忚�堝�?
		file.close();								  // 骞朵笖鍏抽棴鏂囦�?
		return true;								  // 杩斿洖true
	}
	return false; // 濡傛灉鏂囦�?�鏈�鎵惧埌锛屽�?杩斿洖false
}
void handleUserRequet()
{
	// 鑾峰彇鐢ㄦ埛璇锋眰缃戝潃淇℃�?
	String webAddress = esp8266_server.uri();
	// �?氳繃handleFileRead鍑芥暟澶�?�勭悊鐢ㄦ埛璁块�?
	bool fileReadOK = handleFileRead(webAddress);
	// 濡傛灉鍦⊿PIFFS鏃犳硶鎵惧埌�?ㄦ埛璁块棶鐨�?�?婧愶紝鍒欏洖澶�404 (Not Found)
	if (!fileReadOK)
	{
		esp8266_server.send(404, "text/plain", "404 Not Found");
	}
}
void handleRoot_POST()
{
	uint8_t *gb2312_string = (uint8_t *)esp8266_server.arg("gb2312").c_str();
	Serial.println(strlen((const char *)gb2312_string));
	for (int i = 0; i < strlen((const char *)gb2312_string); i++)
	{
		Serial.print(gb2312_string[i], 16);
		Serial.print(" ");
	}
	Serial.println();
	Serial.println("--------------------");
	esp8266_server.send(200, "text/plain", "�յ��ַ���!");
}
void setup()
{
	Serial.begin(115200);
	wifiMulti.addAP("xiaomi123", "a.13513710972");

	Serial.println("Connecting ...");

	int i = 0;
	while (wifiMulti.run() != WL_CONNECTED)
	{
		delay(1000);
		Serial.print(i++);
		Serial.print(' ');
	}

	// WiFi杩炴帴鎴�?姛鍚庡皢�?氳繃涓插彛鐩戣�嗗櫒杈撳�?杩炴帴鎴�?姛淇℃伅
	Serial.println('\n');
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID()); // �?氳繃涓插彛鐩戣�嗗櫒杈撳�?杩炴帴鐨刉iFi鍚嶇О
	Serial.print("IP address:\t");
	Serial.println(WiFi.localIP()); // �?氳繃涓插彛鐩戣�嗗櫒杈撳嚭ESP8266-NodeMCU鐨処P

	if (SPIFFS.begin())
	{ // 鍚�鍔ㄩ�?瀛樻枃浠剁郴缁�
		Serial.println("SPIFFS Started.");
	}
	else
	{
		Serial.println("SPIFFS Failed to Start.");
	}
	esp8266_server.on("/", HTTP_POST, handleRoot_POST);
	esp8266_server.onNotFound(handleUserRequet); // 鍛婄煡绯荤粺濡備綍澶�?悊鐢ㄦ埛璇锋�?

	esp8266_server.begin(); // 鍚�鍔ㄧ綉绔欐湇鍔�
	Serial.println("HTTP server started");
}
void loop(void)
{
	esp8266_server.handleClient();
}




