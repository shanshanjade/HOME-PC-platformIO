#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <PxMatrix.h>
#include <config.h>
#include <asc16.c>
#include <hzk16s.c>
#include <Ticker.h>
Ticker display_ticker;
Ticker text_ticker;
uint8_t display_draw_time = 10; //30-70 is usually fine

void display_updater()
{
	display.display(display_draw_time);
}
void display_update_enable(bool is_enable)
{
	if (is_enable)
		display_ticker.attach(0.002, display_updater);
	else
		display_ticker.detach();
}

GFXcanvas1 myCanvas1(64, 16);
char myword[] = "����һ�ΰ�,�ҵ��й�,����һ����������,China NB!!!";
void text_ticker_callback()
{
	static int text_scroll_offset = 0;

	int i = 0;
	while (i < strlen((char *)myword))
	{
		if (myword[i] > 127)
		{
			unsigned long offset = (94 * (unsigned int)(myword[i] - 0xa0 - 1) + (myword[i + 1] - 0xa0 - 1)) * 32;
			myCanvas1.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &achzk16s[offset], 16, 16, 0b1111100000000000, 0);
			i += 2;
		}
		else
		{
			unsigned long offset = myword[i] * 16;
			myCanvas1.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &acasc16[offset], 8, 16, 0b1111100000000000, 0);
			i++;
		}
	}
	text_scroll_offset++;
	if (text_scroll_offset > strlen((char *)myword) * 8 + 64)
		text_scroll_offset = 0;
	display.drawBitmap(0, 16, myCanvas1.getBuffer(), 64, 16, 0b0000011111100000, 0);
}
ESP8266WebServer myServer(80);
String getContentType(String filename);
bool handleFileRead(String path);
void handleUserRequet();
void handleRoot();
void handleFileUpload();
void respondOK();
void setup()
{
	//��ʼ������
	Serial.begin(115200);
	//��ʼ����Ļ
	display.begin(16);
	display.setBrightness(125);
	display.clearDisplay();
	display.setColorOffset(0, 0, 120);
	display_update_enable(true);
	display.setTextColor(0b1111100000000000);
	display.setTextWrap(false);
	display.setCursor(0, 0);
	display.printf("confing wifi");
	text_ticker.attach(0.05, text_ticker_callback);
	//��ʼ��wifi
	WiFi.mode(WIFI_STA);
	WiFi.begin("xiaomi123", "a.13513710972");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(1000);
	}
	Serial.println();
	Serial.println("WIFI������");
	Serial.println(WiFi.localIP().toString());
	display.setCursor(0, 8);
	display.printf(WiFi.localIP().toString().c_str());
	//��ʼ��SPIFFS
	if (SPIFFS.begin())
		Serial.println("SPIFFS ������");
	else
		Serial.println("SPIFFS ����ʧ��");
	//��ʼ��server
	myServer.on("/", HTTP_POST, handleRoot);
	myServer.on("/upload", HTTP_POST, respondOK, handleFileUpload);
	myServer.onNotFound(handleUserRequet);
	myServer.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	myServer.handleClient();
}
void handleRoot()
{
	Dir myDir = SPIFFS.openDir("/");
	DynamicJsonDocument doc(2048);
	while (myDir.next())
	{
		doc.add(myDir.fileName());
	}
	String temp;
	serializeJson(doc, temp);
	Serial.println(temp);
	myServer.send(200, "text/plain", temp);
	Serial.println(ESP.getFreeHeap());
}
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
{
	if (path.endsWith("/"))
	{
		path = "/index.html";
	}

	String contentType = getContentType(path);

	if (SPIFFS.exists(path))
	{
		File file = SPIFFS.open(path, "r");
		myServer.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}
void handleUserRequet()
{
	String webAddress = myServer.uri();
	Serial.println(webAddress);
	webAddress.replace("%20", " ");
	bool fileReadOK = handleFileRead(webAddress);
	if (!fileReadOK)
	{
		myServer.send(404, "text/plain", "404 Not Found");
	}
}

//�ظ�״̬�� 200 ���ͻ���
void respondOK()
{
	myServer.send(200);
}
File fsUploadFile;
String filename;
// �����ϴ��ļ�����
void handleFileUpload()
{
	HTTPUpload &upload = myServer.upload();
	if (upload.status == UPLOAD_FILE_START)
	{								// ����ϴ�״̬ΪUPLOAD_FILE_START
		filename = upload.filename; // �����ַ����������ڴ���ϴ��ļ���
		if (!filename.startsWith("/"))
			filename = "/" + filename;			   // Ϊ�ϴ��ļ���ǰ����"/"
		Serial.println("File Name: " + filename);  // ͨ�����ڼ���������ϴ��ļ�������
		fsUploadFile = SPIFFS.open(filename, "w"); // ��SPIFFS�н����ļ�����д���û��ϴ����ļ�����
		Serial.println("��ʼд���ļ�");
	}
	else if (upload.status == UPLOAD_FILE_WRITE)
	{ // ����ϴ�״̬ΪUPLOAD_FILE_WRITE
		Serial.println("����д���ļ�......");
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize); // ��SPIFFS�ļ�д��������������ļ�����
	}
	else if (upload.status == UPLOAD_FILE_END)
	{ // ����ϴ�״̬ΪUPLOAD_FILE_END
		Serial.println("д���ļ�����");
		if (fsUploadFile)
		{												  // ����ļ��ɹ�����
			fsUploadFile.close();						  // ���ļ��ر�
			Serial.println(" Size: " + upload.totalSize); // ͨ�����ڼ���������ļ���С
			myServer.send(200, "text/plain", "write file success!");
			Serial.println("�ɹ������ļ�");
			File testFile = SPIFFS.open(filename, "r");
			Serial.println(testFile.readString());
		}
		else
		{																   // ����ļ�δ�ܳɹ�����
			Serial.println("�����ļ�ʧ��");								   // ͨ�����ڼ��������������Ϣ
			myServer.send(500, "text/plain", "500: couldn't create file"); // �������������Ӧ����500������������
		}
	}
}
