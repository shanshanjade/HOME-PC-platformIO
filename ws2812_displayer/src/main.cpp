/*功能展示：
1.显示时间，日期，星期，
2.显示message
3.显示菜单并可以操作
4.各个显示轮番切换
5.数字按键2/4/6/8上下左右控制
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "yellowMatrix.c"
#include "fire.c"
#include "greenMatrixBig.c"
#include "ApcFont35Bitmaps.c"
#include <TimeLib.h>
#include <WiFiUdp.h>

#define PIN D3

byte buffer[20000];

Ticker GIFScroll;
Ticker textScroll;
Ticker matrixShow;
Ticker loopTicker;
Ticker readSerial;

int funcIndex = 0;

uint8_t hours = 2;
uint8_t minites = 59;
uint8_t seconds = 7;
uint8_t mounths = 1;
uint8_t date = 27;
uint8_t weeks = 3;
//-------------------矩阵灯初始化------------------------------------
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
//--------------u16颜色数组---------------------------------------------
const uint16_t colors[6] = {
	matrix.Color(255, 0, 0),
	matrix.Color(255, 255, 0),
	matrix.Color(0, 255, 0),
	matrix.Color(0, 255, 255),
	matrix.Color(0, 0, 255),
	matrix.Color(255, 0, 255)};

uint16_t weekColor = colors[0];
uint16_t dateColor = colors[4];
uint16_t timeColor = colors[3];
uint16_t messageColor = colors[5];
uint16_t systemColor = colors[5];

//-------------------数字字母字体-----------------------------------
const uint8_t ApcFont35Bitmaps[] PROGMEM = {
	0x00,								/* 0x20 space */
	0x80, 0x80, 0x80, 0x00, 0x80,		/* 0x21 exclam */
	0xA0, 0xA0,							/* 0x22 quotedbl */
	0xA0, 0xE0, 0xA0, 0xE0, 0xA0,		/* 0x23 numbersign */
	0x60, 0xC0, 0x60, 0xC0, 0x40,		/* 0x24 dollar */
	0x80, 0x20, 0x40, 0x80, 0x20,		/* 0x25 percent */
	0xC0, 0xC0, 0xE0, 0xA0, 0x60,		/* 0x26 ampersand */
	0x80, 0x80,							/* 0x27 quotesingle */
	0x40, 0x80, 0x80, 0x80, 0x40,		/* 0x28 parenleft */
	0x80, 0x40, 0x40, 0x40, 0x80,		/* 0x29 parenright */
	0xA0, 0x40, 0xA0,					/* 0x2A asterisk */
	0x40, 0xE0, 0x40,					/* 0x2B plus */
	0x40, 0x80,							/* 0x2C comma */
	0xE0,								/* 0x2D hyphen */
	0x80,								/* 0x2E period */
	0x20, 0x20, 0x40, 0x80, 0x80,		/* 0x2F slash */
	0xE0, 0xA0, 0xA0, 0xA0, 0xE0,		/* 0x30 zero */
	0x40, 0xC0, 0x40, 0x40, 0xE0,		/* 0x31 one 0x40, 0x40, 0x40, 0x40, 0x40, */
	0xE0, 0x20, 0xE0, 0x80, 0xE0,		/* 0x32 two */
	0xE0, 0x20, 0xE0, 0x20, 0xE0,		/* 0x33 three */
	0xA0, 0xA0, 0xE0, 0x20, 0x20,		/* 0x34 four */
	0xE0, 0x80, 0xE0, 0x20, 0xE0,		/* 0x35 five */
	0xE0, 0x80, 0xE0, 0xA0, 0xE0,		/* 0x36 six */
	0xE0, 0x20, 0x20, 0x20, 0x20,		/* 0x37 seven */
	0xE0, 0xA0, 0xE0, 0xA0, 0xE0,		/* 0x38 eight */
	0xE0, 0xA0, 0xE0, 0x20, 0xE0,		/* 0x39 nine */
	0x80, 0x00, 0x80,					/* 0x3A colon */
	0x40, 0x00, 0x40, 0x80,				/* 0x3B semicolon */
	0x20, 0x40, 0x80, 0x40, 0x20,		/* 0x3C less */
	0xE0, 0x00, 0xE0,					/* 0x3D equal */
	0x80, 0x40, 0x20, 0x40, 0x80,		/* 0x3E greater */
	0xE0, 0x20, 0x40, 0x00, 0x40,		/* 0x3F question */
	0x40, 0xA0, 0xE0, 0x80, 0x60,		/* 0x40 at */
	0x40, 0xA0, 0xE0, 0xA0, 0xA0,		/* 0x41 A */
	0xC0, 0xA0, 0xC0, 0xA0, 0xC0,		/* 0x42 B */
	0x60, 0x80, 0x80, 0x80, 0x60,		/* 0x43 C */
	0xC0, 0xA0, 0xA0, 0xA0, 0xC0,		/* 0x44 D */
	0xE0, 0x80, 0xE0, 0x80, 0xE0,		/* 0x45 E */
	0xE0, 0x80, 0xE0, 0x80, 0x80,		/* 0x46 F */
	0x60, 0x80, 0xE0, 0xA0, 0x60,		/* 0x47 G */
	0xA0, 0xA0, 0xE0, 0xA0, 0xA0,		/* 0x48 H */
	0xE0, 0x40, 0x40, 0x40, 0xE0,		/* 0x49 I */
	0x20, 0x20, 0x20, 0xA0, 0x40,		/* 0x4A J */
	0xA0, 0xA0, 0xC0, 0xA0, 0xA0,		/* 0x4B K */
	0x80, 0x80, 0x80, 0x80, 0xE0,		/* 0x4C L */
	0xA0, 0xE0, 0xE0, 0xA0, 0xA0,		/* 0x4D M */
	0xA0, 0xE0, 0xE0, 0xE0, 0xA0,		/* 0x4E N */
	0x40, 0xA0, 0xA0, 0xA0, 0x40,		/* 0x4F O */
	0xC0, 0xA0, 0xC0, 0x80, 0x80,		/* 0x50 P */
	0x40, 0xA0, 0xA0, 0xE0, 0x60,		/* 0x51 Q */
	0xC0, 0xA0, 0xE0, 0xC0, 0xA0,		/* 0x52 R */
	0x60, 0x80, 0x40, 0x20, 0xC0,		/* 0x53 S */
	0xE0, 0x40, 0x40, 0x40, 0x40,		/* 0x54 T */
	0xA0, 0xA0, 0xA0, 0xA0, 0x60,		/* 0x55 U */
	0xA0, 0xA0, 0xA0, 0x40, 0x40,		/* 0x56 V */
	0xA0, 0xA0, 0xE0, 0xE0, 0xA0,		/* 0x57 W */
	0xA0, 0xA0, 0x40, 0xA0, 0xA0,		/* 0x58 X */
	0xA0, 0xA0, 0x40, 0x40, 0x40,		/* 0x59 Y */
	0xE0, 0x20, 0x40, 0x80, 0xE0,		/* 0x5A Z */
	0xE0, 0x80, 0x80, 0x80, 0xE0,		/* 0x5B bracketleft */
	0x80, 0x40, 0x20,					/* 0x5C backslash */
	0xE0, 0x20, 0x20, 0x20, 0xE0,		/* 0x5D bracketright */
	0x40, 0xA0,							/* 0x5E asciicircum */
	0xE0,								/* 0x5F underscore */
	0x80, 0x40,							/* 0x60 grave */
	0xC0, 0x60, 0xA0, 0xE0,				/* 0x61 a */
	0x80, 0xC0, 0xA0, 0xA0, 0xC0,		/* 0x62 b */
	0x60, 0x80, 0x80, 0x60,				/* 0x63 c */
	0x20, 0x60, 0xA0, 0xA0, 0x60,		/* 0x64 d */
	0x60, 0xA0, 0xC0, 0x60,				/* 0x65 e */
	0x20, 0x40, 0xE0, 0x40, 0x40,		/* 0x66 f */
	0x60, 0xA0, 0xE0, 0x20, 0x40,		/* 0x67 g */
	0x80, 0xC0, 0xA0, 0xA0, 0xA0,		/* 0x68 h */
	0x80, 0x00, 0x80, 0x80, 0x80,		/* 0x69 i */
	0x20, 0x00, 0x20, 0x20, 0xA0, 0x40, /* 0x6A j */
	0x80, 0xA0, 0xC0, 0xC0, 0xA0,		/* 0x6B k */
	0xC0, 0x40, 0x40, 0x40, 0xE0,		/* 0x6C l */
	0xE0, 0xE0, 0xE0, 0xA0,				/* 0x6D m */
	0xC0, 0xA0, 0xA0, 0xA0,				/* 0x6E n */
	0x40, 0xA0, 0xA0, 0x40,				/* 0x6F o */
	0xC0, 0xA0, 0xA0, 0xC0, 0x80,		/* 0x70 p */
	0x60, 0xA0, 0xA0, 0x60, 0x20,		/* 0x71 q */
	0x60, 0x80, 0x80, 0x80,				/* 0x72 r */
	0x60, 0xC0, 0x60, 0xC0,				/* 0x73 s */
	0x40, 0xE0, 0x40, 0x40, 0x60,		/* 0x74 t */
	0xA0, 0xA0, 0xA0, 0x60,				/* 0x75 u */
	0xA0, 0xA0, 0xE0, 0x40,				/* 0x76 v */
	0xA0, 0xE0, 0xE0, 0xE0,				/* 0x77 w */
	0xA0, 0x40, 0x40, 0xA0,				/* 0x78 x */
	0xA0, 0xA0, 0x60, 0x20, 0x40,		/* 0x79 y */
	0xE0, 0x60, 0xC0, 0xE0,				/* 0x7A z */
	0x60, 0x40, 0x80, 0x40, 0x60,		/* 0x7B braceleft */
	0x80, 0x80, 0x00, 0x80, 0x80,		/* 0x7C bar */
	0xC0, 0x40, 0x20, 0x40, 0xC0,		/* 0x7D braceright */
	0x60, 0xC0,							/* 0x7E asciitilde */
};
const GFXglyph ApcFont35Glyphs[] PROGMEM = {
	{0, 8, 1, 2, 0, -5},   /* 0x20 space */
	{1, 8, 5, 2, 0, -5},   /* 0x21 exclam */
	{6, 8, 2, 4, 0, -5},   /* 0x22 quotedbl */
	{8, 8, 5, 4, 0, -5},   /* 0x23 numbersign */
	{13, 8, 5, 4, 0, -5},  /* 0x24 dollar */
	{18, 8, 5, 4, 0, -5},  /* 0x25 percent */
	{23, 8, 5, 4, 0, -5},  /* 0x26 ampersand */
	{28, 8, 2, 2, 0, -5},  /* 0x27 quotesingle */
	{30, 8, 5, 3, 0, -5},  /* 0x28 parenleft */
	{35, 8, 5, 3, 0, -5},  /* 0x29 parenright */
	{40, 8, 3, 4, 0, -5},  /* 0x2A asterisk */
	{43, 8, 3, 4, 0, -4},  /* 0x2B plus */
	{46, 8, 2, 3, 0, -2},  /* 0x2C comma */
	{48, 8, 1, 4, 0, -3},  /* 0x2D hyphen */
	{49, 8, 1, 2, 0, -1},  /* 0x2E period */
	{50, 8, 5, 4, 0, -5},  /* 0x2F slash */
	{55, 8, 5, 4, 0, -5},  /* 0x30 zero */
	{60, 8, 5, 4, 0, -5},  /* 0x31 one */
	{65, 8, 5, 4, 0, -5},  /* 0x32 two */
	{70, 8, 5, 4, 0, -5},  /* 0x33 three */
	{75, 8, 5, 4, 0, -5},  /* 0x34 four */
	{80, 8, 5, 4, 0, -5},  /* 0x35 five */
	{85, 8, 5, 4, 0, -5},  /* 0x36 six */
	{90, 8, 5, 4, 0, -5},  /* 0x37 seven */
	{95, 8, 5, 4, 0, -5},  /* 0x38 eight */
	{100, 8, 5, 4, 0, -5}, /* 0x39 nine */
	{105, 8, 3, 2, 0, -4}, /* 0x3A colon */
	{108, 8, 4, 3, 0, -4}, /* 0x3B semicolon */
	{112, 8, 5, 4, 0, -5}, /* 0x3C less */
	{117, 8, 3, 4, 0, -4}, /* 0x3D equal */
	{120, 8, 5, 4, 0, -5}, /* 0x3E greater */
	{125, 8, 5, 4, 0, -5}, /* 0x3F question */
	{130, 8, 5, 4, 0, -5}, /* 0x40 at */
	{135, 8, 5, 4, 0, -5}, /* 0x41 A */
	{140, 8, 5, 4, 0, -5}, /* 0x42 B */
	{145, 8, 5, 4, 0, -5}, /* 0x43 C */
	{150, 8, 5, 4, 0, -5}, /* 0x44 D */
	{155, 8, 5, 4, 0, -5}, /* 0x45 E */
	{160, 8, 5, 4, 0, -5}, /* 0x46 F */
	{165, 8, 5, 4, 0, -5}, /* 0x47 G */
	{170, 8, 5, 4, 0, -5}, /* 0x48 H */
	{175, 8, 5, 4, 0, -5}, /* 0x49 I */
	{180, 8, 5, 4, 0, -5}, /* 0x4A J */
	{185, 8, 5, 4, 0, -5}, /* 0x4B K */
	{190, 8, 5, 4, 0, -5}, /* 0x4C L */
	{195, 8, 5, 4, 0, -5}, /* 0x4D M */
	{200, 8, 5, 4, 0, -5}, /* 0x4E N */
	{205, 8, 5, 4, 0, -5}, /* 0x4F O */
	{210, 8, 5, 4, 0, -5}, /* 0x50 P */
	{215, 8, 5, 4, 0, -5}, /* 0x51 Q */
	{220, 8, 5, 4, 0, -5}, /* 0x52 R */
	{225, 8, 5, 4, 0, -5}, /* 0x53 S */
	{230, 8, 5, 4, 0, -5}, /* 0x54 T */
	{235, 8, 5, 4, 0, -5}, /* 0x55 U */
	{240, 8, 5, 4, 0, -5}, /* 0x56 V */
	{245, 8, 5, 4, 0, -5}, /* 0x57 W */
	{250, 8, 5, 4, 0, -5}, /* 0x58 X */
	{255, 8, 5, 4, 0, -5}, /* 0x59 Y */
	{260, 8, 5, 4, 0, -5}, /* 0x5A Z */
	{265, 8, 5, 4, 0, -5}, /* 0x5B bracketleft */
	{270, 8, 3, 4, 0, -4}, /* 0x5C backslash */
	{273, 8, 5, 4, 0, -5}, /* 0x5D bracketright */
	{278, 8, 2, 4, 0, -5}, /* 0x5E asciicircum */
	{280, 8, 1, 4, 0, -1}, /* 0x5F underscore */
	{281, 8, 2, 3, 0, -5}, /* 0x60 grave */
	{283, 8, 4, 4, 0, -4}, /* 0x61 a */
	{287, 8, 5, 4, 0, -5}, /* 0x62 b */
	{292, 8, 4, 4, 0, -4}, /* 0x63 c */
	{296, 8, 5, 4, 0, -5}, /* 0x64 d */
	{301, 8, 4, 4, 0, -4}, /* 0x65 e */
	{305, 8, 5, 4, 0, -5}, /* 0x66 f */
	{310, 8, 5, 4, 0, -4}, /* 0x67 g */
	{315, 8, 5, 4, 0, -5}, /* 0x68 h */
	{320, 8, 5, 2, 0, -5}, /* 0x69 i */
	{325, 8, 6, 4, 0, -5}, /* 0x6A j */
	{331, 8, 5, 4, 0, -5}, /* 0x6B k */
	{336, 8, 5, 4, 0, -5}, /* 0x6C l */
	{341, 8, 4, 4, 0, -4}, /* 0x6D m */
	{345, 8, 4, 4, 0, -4}, /* 0x6E n */
	{349, 8, 4, 4, 0, -4}, /* 0x6F o */
	{353, 8, 5, 4, 0, -4}, /* 0x70 p */
	{358, 8, 5, 4, 0, -4}, /* 0x71 q */
	{363, 8, 4, 4, 0, -4}, /* 0x72 r */
	{367, 8, 4, 4, 0, -4}, /* 0x73 s */
	{371, 8, 5, 4, 0, -5}, /* 0x74 t */
	{376, 8, 4, 4, 0, -4}, /* 0x75 u */
	{380, 8, 4, 4, 0, -4}, /* 0x76 v */
	{384, 8, 4, 4, 0, -4}, /* 0x77 w */
	{388, 8, 4, 4, 0, -4}, /* 0x78 x */
	{392, 8, 5, 4, 0, -4}, /* 0x79 y */
	{397, 8, 4, 4, 0, -4}, /* 0x7A z */
	{401, 8, 5, 4, 0, -5}, /* 0x7B braceleft */
	{406, 8, 5, 2, 0, -5}, /* 0x7C bar */
	{411, 8, 5, 4, 0, -5}, /* 0x7D braceright */
	{416, 8, 2, 4, 0, -5}, /* 0x7E asciitilde */
};
const GFXfont ApcFont35 PROGMEM = {(uint8_t *)ApcFont35Bitmaps, (GFXglyph *)ApcFont35Glyphs, 0x20, 0x7E, 6};
//-------------------NTP和UDP-----------------------------------
// NTP Servers:
static const char ntpServerName[] = "ntp1.aliyun.com";
const int timeZone = 8;
WiFiUDP Udp;
unsigned int localPort = 8888; // local port to listen for UDP packets
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48;		// NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
time_t getNtpTime()
{
	IPAddress ntpServerIP; // NTP server's ip address

	while (Udp.parsePacket() > 0)
		; // discard any previously received packets
	Serial.println("Transmit NTP Request");
	// get a random server from the pool
	WiFi.hostByName(ntpServerName, ntpServerIP);
	Serial.print(ntpServerName);
	Serial.print(": ");
	Serial.println(ntpServerIP);
	sendNTPpacket(ntpServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500)
	{
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE)
		{
			Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
		}
	}
	Serial.println("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}
// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011; // LI, Version, Mode
	packetBuffer[1] = 0;		  // Stratum, or type of clock
	packetBuffer[2] = 6;		  // Polling Interval
	packetBuffer[3] = 0xEC;		  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}



//-------------------显示星期--------------------------------
void drawWeek(uint8_t _week)
{
	for (uint8_t i = 0; i < 31; i++)
	{
		matrix.drawPixel(i, 7, 0xffff);
	};
	matrix.drawPixel(0, 7, 0x00);
	matrix.drawPixel(4, 7, 0x00);
	matrix.drawPixel(8, 7, 0x00);
	matrix.drawPixel(12, 7, 0x00);
	matrix.drawPixel(16, 7, 0x00);
	matrix.drawPixel(20, 7, 0x00);
	matrix.drawPixel(21, 7, 0x00);
	matrix.drawPixel(26, 7, 0x00);
	switch (_week)
	{
	case 2:
		matrix.drawPixel(1, 7, weekColor); //星期一
		matrix.drawPixel(2, 7, weekColor);
		matrix.drawPixel(3, 7, weekColor);
		break;
	case 3:
		matrix.drawPixel(5, 7, weekColor); //星期二
		matrix.drawPixel(6, 7, weekColor);
		matrix.drawPixel(7, 7, weekColor);
		break;
	case 4:
		matrix.drawPixel(9, 7, weekColor); //星期三
		matrix.drawPixel(10, 7, weekColor);
		matrix.drawPixel(11, 7, weekColor);
		break;
	case 5:
		matrix.drawPixel(13, 7, weekColor); //星期四
		matrix.drawPixel(14, 7, weekColor);
		matrix.drawPixel(15, 7, weekColor);
		break;
	case 6:
		matrix.drawPixel(17, 7, weekColor); //星期五
		matrix.drawPixel(18, 7, weekColor);
		matrix.drawPixel(19, 7, weekColor);
		break;
	case 7:
		matrix.drawPixel(22, 7, weekColor); //星期六
		matrix.drawPixel(23, 7, weekColor);
		matrix.drawPixel(24, 7, weekColor);
		matrix.drawPixel(25, 7, weekColor);
		break;
	case 1:
		matrix.drawPixel(27, 7, weekColor); //星期日
		matrix.drawPixel(28, 7, weekColor);
		matrix.drawPixel(29, 7, weekColor);
		matrix.drawPixel(30, 7, weekColor);
		break;
	}
}
//-------------------显示文字及滚动--------------------------------
int cursorX = 0;
unsigned int oneSecond = 0;
int flag = -1;
void textScroll_callback(char *message)
{
	matrix.clear();
	oneSecond++;
	matrix.setCursor(cursorX, 6);
	matrix.print(message);
	if (oneSecond > 40 && strlen(message) * 4 > 31)
		cursorX += flag;
	if (cursorX < -((int)strlen(message) * 4 - 31) || cursorX > 1)
	{
		flag = -flag;
	}
}

void textTime_callback()
{
	matrix.clear();
	matrix.setCursor(3, 6);
	matrix.setTextColor(timeColor);
	matrix.printf("%02d:%02d:%02d", hours, minites, seconds);
	drawWeek(weeks);
}

void textDate_callback()
{
	matrix.clear();
	matrix.setCursor(6, 6);
	matrix.setTextColor(dateColor);
	matrix.printf("%02d-%02d", mounths, date);
	drawWeek(weeks);
}

void showTEXT(char *message, uint16_t color)
{
	flag = -1;
	cursorX = 0;
	oneSecond = 0;
	matrix.setTextColor(color);
	if (textScroll.active())
	{
		textScroll.detach();
	}
	if (GIFScroll.active())
	{
		GIFScroll.detach();
	}
	textScroll.attach(0.1, textScroll_callback, message);
}
String ipaddr;
//------------------显示菜单及滚动---------------------------------
void showMenu(char *message)
{
	flag = -1;
	cursorX = 0;
	oneSecond = 0;
	matrix.setTextColor(systemColor);
	if (textScroll.active())
	{
		textScroll.detach();
	}
	if (GIFScroll.active())
	{
		GIFScroll.detach();
	}
	if (loopTicker.active())
	{
		loopTicker.detach();
	}
	textScroll.attach(0.1, textScroll_callback, message);
	Serial.println("func showMenu");
}
//------------------显示GIF及滚动---------------------------------
uint8_t width;
uint8_t height;
uint8_t frames;
uint32_t delayMs;
uint16_t size_data;
uint16_t *u16Data;
void GIFScroll_callback(void *myVoid)
{
	byte *tempGIF = (byte *)myVoid;
	static uint8_t index = 0;
	matrix.drawRGBBitmap(0, 0, &u16Data[index * width * height], width, height);
	index++;
	if (index > frames - 1)
		index = 0;
}
void showGIF(const unsigned char *tempGIF)
{
	width = pgm_read_byte(&tempGIF[0]);
	height = pgm_read_byte(&tempGIF[1]);
	frames = pgm_read_byte(&tempGIF[2]);
	delayMs = pgm_read_byte(&tempGIF[3]);
	size_data = width * height * 2 * frames;
	// u16Data = (uint16_t *)memcpy_P(malloc(size_data), &tempGIF[8], size_data);
	u16Data = (uint16_t *)memcpy_P(buffer, &tempGIF[8], size_data);

	matrix.clear();
	if (textScroll.active())
	{
		textScroll.detach();
	}
	if (GIFScroll.active())
	{
		GIFScroll.detach();
	}
	void *myVoid = (void *)tempGIF;
	GIFScroll.attach_ms(delayMs, GIFScroll_callback, myVoid);
	// free(u16Data);
	Serial.println("func showGIF");
}
//-----------------时间星期 日期------------------------------

void showTime()
{

	if (textScroll.active())
		textScroll.detach();
	if (GIFScroll.active())
		GIFScroll.detach();
	Serial.println("showTime func");
	textScroll.attach(0.1, textTime_callback);
}
void showDate()
{
	if (textScroll.active())
	{
		textScroll.detach();
	}
	if (GIFScroll.active())
		GIFScroll.detach();
	textScroll.attach(0.1, textDate_callback);
}
//-----------------图片文字轮播------------------------------
void loopTicker_callback()
{
	Serial.println("loop ticker callback");
	Serial.print("freeheaps:");
	Serial.println(ESP.getFreeHeap());
	static uint8_t loopIndex = 0;
	switch (loopIndex)
	{
	case 0:
		Serial.println("showTime");
		showTime();
		// drawWeek(3);
		break;
	case 1:
		Serial.println("showGIF");
		showGIF(gImage_fire);
		break;
	case 2:
		Serial.println("showDate");
		showDate();
		break;
	case 3:
		Serial.println("showGIF");
		showGIF(gImage_greenMatrixBig);
		break;
	case 4:
		Serial.println("showMessage");
		showTEXT("Message", messageColor);
		break;
	case 5:
		Serial.println("showGIF");
		showGIF(gImage_yellowMatrix);
		break;
	}
	loopIndex++;
	if (loopIndex > 5)
		loopIndex = 0;
}
//-----------------屏幕刷新------------------------------
void matrixShow_callback() { matrix.show(); }
//-----------------菜单结构----------------------------------
int systemColorIndex = 0;
typedef struct keycodeStruct
{
	uint8_t keyStateIndex;
	uint8_t keyDownState;
	uint8_t keyUpState;
	uint8_t keyEnterState;
	uint8_t keyEscState;
	char *menuString;
	void (*operation)(char *menuString);
};
void menu41(char *menuString)
{
	if(menuString == "DATE COLOR"){
		static uint8_t dateColor_index = 0;
		dateColor_index++ ;
		dateColor = colors[dateColor_index];
	}
	else if (menuString == "TIME COLOR")
	{
		static uint8_t timeColor_index = 0;
		timeColor_index++ ;
		timeColor = colors[timeColor_index];
	}
	else if (menuString == "WEEK COLOR")
	{
		static uint8_t weekColor_index = 0;
		weekColor_index++ ;
		weekColor = colors[weekColor_index];
	}
	else if (menuString == "SYSTEM COLOR")
	{
		static uint8_t systemColor_index = 0;
		systemColor_index++ ;
		systemColor = colors[systemColor_index];
	}
	else if (menuString == "MESSAGE COLOR")
	{
		static uint8_t messageColor_index = 0;
		messageColor_index++ ;
		messageColor = colors[messageColor_index];
	}
	
};
void exit1(char * menuString)
{
	delay(500);
	textScroll.detach();
	funcIndex = 0;
	loopTicker_callback();
	loopTicker.attach(10, loopTicker_callback);
};
void empty(char * menuString){};
keycodeStruct myMenu[13] = {
	//序 下 上 右 左
	{0, 1, 0, 4, 12, "TEXT COLOR", *empty},
	{1, 2, 1, 6, 12, "CLOCK COLOR", *empty},
	{2, 3, 1, 11, 12, "BRIGHT", *empty},
	{3, 0, 2, 11, 12, "DELAY", *empty},

	{4, 5, 5, 11, 0, "SYSTEM COLOR", *empty},
	{5, 4, 4, 11, 0, "MESSAGE COLOR", *empty},

	{6, 7, 6, 11, 1, "DATE COLOR", *empty},
	{7, 8, 6, 11, 1, "TIME COLOR", *empty},
	{8, 8, 7, 11, 1, "WEEK COLOR", *empty},

	{9, 10, 9, 11, 2, "MINBRIGHT", *empty},
	{10, 10, 9, 11, 2, "MAXBRIGHT", *empty},

	{11, 11, 0, 11, 0, "SET COLOR", *menu41},

	{12, 12, 12, 12, 12, "EXIT", *exit1}};


//-----------------定时读取串口----------------------------------

//--------------------------------------------------------
void setup()
{
	Serial.begin(115200);
	delay(1000);
	matrix.begin();
	matrix.setTextWrap(false);
	matrix.setBrightness(10);
	matrix.setTextColor(colors[1]);
	matrix.setFont(&ApcFont35);
	matrix.setCursor(0, 6);
	String myString = "  :ARTRIX";
	matrix.print(myString);
	matrix.show();

	delay(5000);
	matrixShow.attach_ms(30UL, matrixShow_callback);
	WiFi.begin("xiaomi123", "a.13513710972");
	showTEXT("->xiaomi123", colors[0]);
	delay(1000);
	showTEXT("LINKING", colors[1]);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
	}
	showTEXT("WIFI DONE!", colors[2]);
	delay(2000);

	Udp.begin(localPort);
	Serial.println("NTP SYNC");
	setSyncProvider(getNtpTime);
	setSyncInterval(300);

	

	showTEXT("ENJOY:)", colors[0]);
	delay(2000);
	ipaddr = "IP:" + WiFi.localIP().toString();
	Serial.println(ipaddr);
	showTEXT((char *)ipaddr.c_str(), colors[2]);
	delay(1000);
	loopTicker.attach(10, loopTicker_callback);
}

void loop()
{
	if (Serial.available() > 0)
	{
		int keyvalue = Serial.parseInt();
		Serial.flush();
		if (loopTicker.active())
		{
			Serial.println("loopTicker.active()");
			switch (keyvalue)
			{
			case 2:
				funcIndex = myMenu[funcIndex].keyDownState;
				Serial.println(myMenu[funcIndex].menuString);
				showMenu(myMenu[funcIndex].menuString);
				(*myMenu[funcIndex].operation)(myMenu[funcIndex].menuString);
				break;
			case 8:
				funcIndex = myMenu[funcIndex].keyUpState;
				Serial.println(myMenu[funcIndex].menuString);
				showMenu(myMenu[funcIndex].menuString);
				(*myMenu[funcIndex].operation)(myMenu[funcIndex].menuString);
				break;
			case 4:
				loopTicker_callback();
				loopTicker.attach(10, loopTicker_callback);
				Serial.println("loopTicker reattach");
				break;
			case 6:
				loopTicker_callback();
				loopTicker.attach(10, loopTicker_callback);
				Serial.println("loopTicker reattach");
				break;
			}
		}
		else
		{
			switch (keyvalue)
			{
				Serial.println("进入menu");
			case 2:
				funcIndex = myMenu[funcIndex].keyDownState;
				break;
			case 8:
				funcIndex = myMenu[funcIndex].keyUpState;
				break;
			case 6:
				funcIndex = myMenu[funcIndex].keyEnterState;
				break;
			case 4:
				funcIndex = myMenu[funcIndex].keyEscState;
				break;
			}
			Serial.println(myMenu[funcIndex].menuString);
			showMenu(myMenu[funcIndex].menuString);
			(*myMenu[funcIndex].operation)(myMenu[funcIndex].menuString);
		}
	}
	if (timeStatus() != timeNotSet)
	{
		static time_t prevDisplay = 0; // when the digital clock was displayed
		if (now() != prevDisplay)
		{ //update the display only if time has changed
			prevDisplay = now();
			hours = hour();
			minites = minute();
			seconds = second();
			mounths = month();
			date = day();
			weeks = weekday();
		}
	}
	ESP.wdtFeed();
}