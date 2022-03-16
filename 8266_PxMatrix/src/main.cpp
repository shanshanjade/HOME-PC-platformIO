#include <Arduino.h>
#include <FS.h>
#include <PxMatrix.h>
#include <GifPlayer.h>
#include <BmpDecoder.h>
#include <ApcFont.h>
#include <Tiny3x3a2pt7b.h>
#include "hzk16s.c"
#include "asc16.c"
GifPlayer gifPlayer;

#include <Ticker.h>
Ticker display_ticker;
Ticker text_ticker;

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
WiFiUDP Udp;
NTPClient timeClient(Udp, "ntp1.aliyun.com");

uint8_t display_draw_time = 10; //30-70 is usually fine // display_draw_time:10 brightness:128

String file_name;
Dir myDir;

GFXcanvas1 myCanvas1_text(32, 16);
GFXcanvas16 myCanvas16_time(32, 16);

char myword[55] = "���ɵ����ǳ���";

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
void drawWeek(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t bg);
void text_ticker_callback()
{
	unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;

	myCanvas16_time.setFont(&ApcFont35);
	myCanvas16_time.setTextWrap(false);
	myCanvas16_time.fillScreen(0);

	myCanvas16_time.setTextColor(0xFF00);
	myCanvas16_time.setCursor(0, 6);
	myCanvas16_time.printf(" %02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
	myCanvas16_time.setTextColor(0x00FF);
	myCanvas16_time.setCursor(0, 12);
	myCanvas16_time.printf("%02d-%02d-%02d", currentYear % 100, currentMonth, monthDay);

	display.drawRGBBitmap(32, 0, myCanvas16_time.getBuffer(), 32, 16);

	drawWeek(timeClient.getDay()+1, 32, 13, 0xFF00, 0xFFFF);
	drawWeek(timeClient.getDay()+1, 32, 14, 0xFF00, 0xFFFF);

	static int text_scroll_offset = 0;
	int i = 0;
	while (i < strlen((char *)myword))
	{
		if (myword[i] > 127)
		{
			unsigned long offset = (94 * (unsigned int)(myword[i] - 0xa0 - 1) + (myword[i + 1] - 0xa0 - 1)) * 32;
			myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 32, 0, &achzk16s[offset], 16, 16, 0b1111100000000000, 0);
			i += 2;
		}
		else
		{
			unsigned long offset = myword[i] * 16;
			myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 32, 0, &acasc16[offset], 8, 16, 0b1111100000000000, 0);
			i++;
		}
	}
	text_scroll_offset++;
	if (text_scroll_offset > strlen((char *)myword) * 8 + 32)
		text_scroll_offset = 0;
	display.drawBitmap(32, 16, myCanvas1_text.getBuffer(), 32, 16, 0b0000011111100000, 0);
}

void setup()
{
	Serial.begin(115200);
	display.begin(16);
	display.setBrightness(128);
	display.clearDisplay();
	display.setColorOffset(0, 0, 110);
	display_update_enable(true);
	WiFi.mode(WIFI_STA);
	WiFi.begin(F("xiaomi123"), F("a.13513710972"));
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	timeClient.begin();
	timeClient.setTimeOffset(28800);
	timeClient.update();

	text_ticker.attach(0.05, text_ticker_callback);
	SPIFFS.begin();
	myDir = SPIFFS.openDir("/");
	myDir.next();
	file_name = myDir.fileName();
}


void loop()
{
	static uint8_t mode_index = 1;
	if (Serial.available() > 0)
		file_name = Serial.readString();

	File imageFile = SPIFFS.open(file_name, "r");
	if (!file_name.startsWith("/"))
		file_name = "/" + file_name;
	if (file_name.endsWith(".gif") || file_name.endsWith(".GIF"))
		mode_index = 0;
	else if (file_name.endsWith(".bmp") || file_name.endsWith(".BMP"))
		mode_index = 1;
	if (mode_index == 0)
	{
		unsigned long time_point = millis();
		while ((millis() - time_point) <= 6000)
		{
			uint32_t result;
			if (!imageFile)
			{
				Serial.println(F("Failed to open"));
				return;
			}

			gifPlayer.setFile(imageFile);
			if (!gifPlayer.parseGifHeader())
			{
				imageFile.close();
				Serial.println(F("No gif header"));
				return;
			}
			display.fillRect(0, 0, 32, 32, 0);
			gifPlayer.parseLogicalScreenDescriptor();
			gifPlayer.parseGlobalColorTable();
			do
			{
				gifPlayer.drawFrame();
				result = gifPlayer.drawFrame();
			} while (result != ERROR_FINISHED);
			imageFile.seek(0);
			// ESP.wdtFeed();
		}
	}
	else if (mode_index == 1)
	{
		BmpDecoder myBmpDecoder(imageFile);
		display.fillRect(0, 0, 32, 32, 0);
		unsigned long time_point = millis();
		while (millis() - time_point <= 6000)
		{
			display.drawRGBBitmap(0, 0, myBmpDecoder.BitMap_buffer, myBmpDecoder.G_biWidth, myBmpDecoder.G_biHeight);
		}
	}

	if (myDir.next())
		file_name = myDir.fileName();
	else
		myDir = SPIFFS.openDir("/");
	// strcpy((char *)myword, (const char *)file_name.c_str());

	
}

void drawWeek(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t bg)
{
	for (uint8_t i = 0; i < 31; i++)
	{
		display.drawPixel(x + i, y, bg);
	};
	display.drawPixel(x + 0, y, 0x00);
	display.drawPixel(x + 4, y, 0x00);
	display.drawPixel(x + 8, y, 0x00);
	display.drawPixel(x + 12, y, 0x00);
	display.drawPixel(x + 16, y, 0x00);
	display.drawPixel(x + 20, y, 0x00);
	display.drawPixel(x + 21, y, 0x00);
	display.drawPixel(x + 26, y, 0x00);
	switch (_week)
	{
	case 2:
		display.drawPixel(x + 1, y, color); //����һ
		display.drawPixel(x + 2, y, color);
		display.drawPixel(x + 3, y, color);
		break;
	case 3:
		display.drawPixel(x + 5, y, color); //���ڶ�
		display.drawPixel(x + 6, y, color);
		display.drawPixel(x + 7, y, color);
		break;
	case 4:
		display.drawPixel(x + 9, y, color); //������
		display.drawPixel(x + 10, y, color);
		display.drawPixel(x + 11, y, color);
		break;
	case 5:
		display.drawPixel(x + 13, y, color); //������
		display.drawPixel(x + 14, y, color);
		display.drawPixel(x + 15, y, color);
		break;
	case 6:
		display.drawPixel(x + 17, y, color); //������
		display.drawPixel(x + 18, y, color);
		display.drawPixel(x + 19, y, color);
		break;
	case 7:
		display.drawPixel(x + 22, y, color); //������
		display.drawPixel(x + 23, y, color);
		display.drawPixel(x + 24, y, color);
		display.drawPixel(x + 25, y, color);
		break;
	case 1:
		display.drawPixel(x + 27, y, color); //������
		display.drawPixel(x + 28, y, color);
		display.drawPixel(x + 29, y, color);
		display.drawPixel(x + 30, y, color);
		break;
	}
}

