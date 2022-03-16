#define douoble_buffer true

#include <Arduino.h>
#include <FS.h>
#include <PxMatrix.h>
#include <GifPlayer.h>
#include <BmpDecoder.h>
#include <Snake.h>
#include <Font_ApcFont.h>
#include <Font_Asc16.h>
#include <Font_Hzk16s.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <config.h>
#include <ArduinoJson.h>
#include <UTF8toGB2312.c>


// 心知天气HTTP请求所需信息
const char* host 			= "api.seniverse.com";
const char* xinzhi_Key 		= "SN_WBgcbixJJwpJdi"; // 私钥
const char* xinzhi_Location 	= "郑州";		 // 城市
const char* xinzhi_Unit 		= "c";					 // c摄氏/华氏
String stringBuffer;

GifPlayer gifPlayer;
BmpDecoder myBmpDecoder;
uint8_t display_draw_time = 70; //30-70 is usually fine // display_draw_time:10 brightness:128
Ticker display_ticker;
Ticker serial_ticker;

WiFiUDP Udp;
NTPClient timeClient(Udp, "ntp1.aliyun.com");

String file_name;
Dir myDir;

char myword[400] ;

const char flash_String[30][352] PROGMEM = {
	{"《道德经》 老子 第一章 道可道，非常道；名可名，非常名。无名，天地之始，有名，万物之母。故常无欲，以观其妙，常有欲，以观其徼。此两者，同出而异名，同谓之玄，玄之又玄，众妙之门。"},
	{"《道德经》 老子 第二章 天下皆知美之为美，斯恶已；皆知善之为善，斯不善已。故有无相生，难易相成，长短相较，高下相倾，音声相和，前后相随。是以圣人处无为之事，行不言之教，万物作焉而不辞，生而不有，为而不恃，功成而弗居。夫唯弗居，是以不去。"},
	{"《道德经》 老子 第三章 不尚贤，使民不争；不贵难得之货，使民不为盗；不见可欲，使民心不乱。是以圣人之治，虚其心，实其腹；弱其志，强其骨。常使民无知无欲，使夫智者不敢为也。为无为，则无不治。"},
	{"《道德经》 老子 第四章 道冲而用之或不盈，渊兮似万物之宗。挫其锐，解其纷，和其光，同其尘。湛兮似或存，吾不知谁之子，象帝之先。"},
	{"《道德经》 老子 第五章 天地不仁，以万物为刍狗；圣人不仁，以百姓为刍狗。天地之间，其犹橐龠乎？虚而不屈，动而愈出。多言数穷，不如守中。"},
	{"《道德经》 老子 第六章 谷神不死，是谓玄牝，玄牝之门，是谓天地根。绵绵若存，用之不勤。"},
	{"《道德经》 老子 第七章 天长地久。天地所以能长且久者，以其不自生，故能长生。是以圣人后其身而身先，外其身而身存。非以其无私邪？故能成其私。"},
	{"《道德经》 老子 第八章 上善若水。水善利万物而不争，处众人之所恶，故几于道。居善地，心善渊，与善仁，言善信，正善治，事善能，动善时。夫唯不争，故无尤。"},
	{"《道德经》 老子 第九章 持而盈之，不如其已。揣而锐之，不可长保。金玉满堂，莫之能守。富贵而骄，自遗其咎。功成身退，天之道。"},
	{"《道德经》 老子 第十章 载营魄抱一，能无离乎？专气致柔，能婴儿乎？涤除玄览，能无疵乎？爱民治国，能无知乎？天门开阖，能无雌乎？明白四达，能无为乎？生之、畜之，生而不有，为而不恃，长而不宰，是谓玄德。"},
};

// bool is_mode_changed_flag = false;
void SnakeGame_Callback();
void Serial_Ticker_Callback();
void FillRainbow_Callback();
void GifBmp_Callback();
void Text_Callback(char *myString, bool is_rainbow_bg);
void FullDate_Callback();

void httpRequest_Callback(bool is_rainbow_bg);
void parseWeather(WiFiClient client);
void DrawWeek(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t weekend_color, uint16_t bg);
void MyPrintf(char *text, int x, int y, int _offsetX, int _offsetY, uint16_t color);
void setup()
{
	Serial.begin(115200);
	display.begin(16                                                                           );
	
	display.setBrightness(128);
	display.setColorOffset(0, 0, 100); //best (0, 0, 110)
	display.setTextColor(0x07E0);
	display.setTextWrap(false);
	display.setFastUpdate(false);

	display_ticker.attach_ms(5, [](){ display.display(10);});
							 //default is 5
	serial_ticker.attach_ms_scheduled_accurate(500, Serial_Ticker_Callback);

	display.clearDisplay();


	WiFi.mode(WIFI_STA);
	WiFi.begin(F("xiaomi123"), F("a.13513710972"));
	MyPrintf("WiFi", 0, 0, (64 - 4 * 8) / 2, 0, 0xF800);
	int a = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(20);
		MyPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>>>", -28 * 8, 16, a++, 0, 0x07E0);
	}
	display.fillScreen(0);
	MyPrintf("WiFi", 0, 0, (64 - 4 * 8) / 2, 0, 0x07E0);
	MyPrintf("DONE!", 0, 16, (64 - 5 * 8) / 2, 0, 0x07E0);
	delay(2000);
	MyPrintf("ENJOY:)", 0, 16, (64 - 7 * 8) / 2, 0, 0xF81E);
	delay(2000);
	timeClient.begin();
	timeClient.setTimeOffset(28800); //???????
	timeClient.setUpdateInterval(60);
	timeClient.update();
	
	SPIFFS.begin();
	myDir = SPIFFS.openDir("/");
	myDir.next();
	file_name = myDir.fileName();
	
}
void loop()
{
    static int a = 1; 
	SnakeGame_Callback();
	FullDate_Callback();
	Text_Callback((char *)flash_String[a], false);
	GifBmp_Callback();
	httpRequest_Callback(true);
	// FillRainbow_Callback();
	timeClient.update();
    a++;
    if (a > 10)
        a=0;
	Serial.println(ESP.getFreeHeap());
}

//****************GifBmp_Callback*********************
void GifBmp_Callback() 
{
	static uint8_t mode_index = 1;
	File imageFile = SPIFFS.open(file_name, "r");
	if (!file_name.startsWith("/"))
		file_name = "/" + file_name;
	if (file_name.endsWith(".gif") || file_name.endsWith(".GIF"))
		mode_index = 0;
	else if (file_name.endsWith(".bmp") || file_name.endsWith(".BMP"))
		mode_index = 1;
	if (mode_index == 0)
	{
		Serial.println("gif mode");
		display.fillScreen(0);
		unsigned long time_point = millis();
		while ((millis() - time_point) <= 8000)
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
			gifPlayer.parseLogicalScreenDescriptor();
			gifPlayer.parseGlobalColorTable();
			do
			{
				gifPlayer.drawFrame();
				result = gifPlayer.drawFrame();
				//use for double_buffer
			} while (result != ERROR_FINISHED);
			imageFile.seek(0);
		}
	}
	else if (mode_index == 1)
	{
		Serial.println("bmp mode");
		myBmpDecoder.setFile(imageFile);
		display.fillScreen(0);
		unsigned long time_point = millis();
		int bmp_scroll_offset;
		int direction = -1;
		while (millis() - time_point <= 15000)
		{
			unsigned long mydelays = millis();
			if (myBmpDecoder.G_biHeight <= 32)
			{
				display.drawRGBBitmap((64 - myBmpDecoder.G_biWidth) / 2, (32 - myBmpDecoder.G_biHeight) / 2, myBmpDecoder.BitMap_buffer, myBmpDecoder.G_biWidth, myBmpDecoder.G_biHeight);
			}
			else
			{
				display.drawRGBBitmap((64 - myBmpDecoder.G_biWidth) / 2, 0 + bmp_scroll_offset, myBmpDecoder.BitMap_buffer, myBmpDecoder.G_biWidth, myBmpDecoder.G_biHeight);
			}
			bmp_scroll_offset = bmp_scroll_offset + direction;
			if (bmp_scroll_offset <= 32 - myBmpDecoder.G_biHeight)
				direction = 1;
			else if (bmp_scroll_offset >= 0)
				direction = -1;
			
			// int temp = millis()- mydelays;
			// if(temp <= 20)
			// 	temp = 20 - temp;
			// else 
			// 	temp = 1;
			// delay(temp);
			//use for double_buffer
			delay(10);
		}
		myBmpDecoder.free();
	}

	if (myDir.next())
		file_name = myDir.fileName();
	else
		myDir = SPIFFS.openDir("/");
}
void Text_Callback(char *myString, bool is_rainbow_bg)
{
	if (strcmp_P(myword, myString) != 0)
		strcpy(myword, (const char *)myString);
	// strcpy_P(utf8charArr, (const char *)myString);
	GetGb2312String(myword, myword);

	GFXcanvas1 myCanvas1_text(64, 16);
	GFXcanvas16 myCanvas16_text(64, 16);
	display.fillScreen(0);
	int text_scroll_offset = 0;
	int myword_len = strlen((char *)myword);
	int myword_width = strlen((char *)myword) * 8 + 64 - 1;
	//获得渐变背景的canvas buffer；
	for (int i = 0; i < 64 + 16; i++)
		myCanvas16_text.drawLine(i, 0, -16 + i, 16, display.colorHSV((i + text_scroll_offset) * 3, 255, 255));
	//读取点阵字库并 滚动显示
	while (text_scroll_offset <= myword_width)
	{
		unsigned long time_point = millis();
		int i = 0;
		while (i < myword_len)
		{
			if (myword[i] > 127)
			{
				unsigned long offset = (94 * (unsigned int)(myword[i] - 0xa0 - 1) + (myword[i + 1] - 0xa0 - 1)) * 32;
				myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &Hzk16s[offset], 16, 16, 0xF800, 0);
				i += 2;
			}
			else
			{
				unsigned long offset = myword[i] * 16;
				myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &Asc16[offset], 8, 16, 0x07E0, 0);
				i++;
			}
		}
		text_scroll_offset++;
		// display.fillRect(0, 0, 64, 32, 0);
		if (is_rainbow_bg){
			//绘制渐变背景放在这里，可以绘制动态彩色文字效果，但比较卡顿
			for (int i = 0; i < 64 + 16; i++)  
			{
				myCanvas16_text.drawLine(i, 0, -16 + i, 16, display.colorHSV((i + text_scroll_offset) * 3, 255, 255));
			}
			display.drawRGBBitmap(0, 8, myCanvas16_text.getBuffer(), 64, 16);
			display.drawBitmapInvert(0, 8, myCanvas1_text.getBuffer(), 64, 16, 0); 
		}
		else
		{
			display.drawBitmap(0, 8, myCanvas1_text.getBuffer(), 64, 16, 0X001f, 0);
		}
		
		uint16_t temp = millis() - time_point;
		// Serial.println(temp);
		if (temp <= 20)
			temp = 20 - temp;
		else
			temp = 1;
		delay(temp);	
	}
}
void FullDate_Callback()
{
	timeClient.update();
	unsigned long epochTime = timeClient.getEpochTime();
	// Serial.println(epochTime);
	struct tm* p_tm = gmtime((time_t *)&epochTime);
	int monthDay = p_tm->tm_mday;
	int currentMonth = p_tm->tm_mon + 1;
	int currentYear = p_tm->tm_year + 1900;
	// Serial.printf("年%d,月%d,日%d,时%d,分%d,秒%d\n", currentYear, currentMonth, monthDay, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
	String dayStr = monthDay < 10 ? "0" + String(monthDay) : String(monthDay);
	String monthStr = currentMonth < 10 ? "0" + String(currentMonth) : String(currentMonth);
	String yearStr = String(currentYear % 100);
	String formattedDate = yearStr + "/" + monthStr + "/" + dayStr;
	unsigned long time_point = millis();
	display.fillScreen(0);
	MyPrintf((char *)formattedDate.c_str(), 0, -1, 0, 0, 0x03E0);
	DrawWeek(timeClient.getDay() + 1, 0, 29, 0x07FF, 0xf800, display.color565(40, 40, 150));
	while (millis() - time_point <= 10000)
	{
		MyPrintf((char *)timeClient.getFormattedTime().c_str(), 0, 13, 0, 0, 0xD100);
		delay(50);
	}
	Serial.println(epochTime);
}
void FillRainbow_Callback()
{
	display.fillScreen(0);
	unsigned long time = millis();
	while (millis() - time <= 5000)
	{
		for (int i = 0; i < 32; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				// display.fillScreen(0);
				display.drawPixel(j-1, i, 0);
				display.drawPixel(j, i, 0x001F);
				delay(1);

			}
		}
	}
}

//****************Tick Callback*********************
void Serial_Ticker_Callback()
{
	static int a = 0;
	static int b = 0;
	static int brightness = 0;
	if (Serial.available() > 0)
	{
		String serialstring = Serial.readString();

		if (serialstring.endsWith("b"))
		{
			brightness = serialstring.toInt();
			display.setBrightness(brightness); //亮度测试
		}
		else if (serialstring.endsWith("*"))
		{
			a = serialstring.toInt();
			display_ticker.detach();
			display_ticker.attach_ms(a, []()
							 { display.display(b); });
		}
		
		else if (serialstring.endsWith("-"))
		{
			b = serialstring.toInt();
			display_ticker.detach();
			display_ticker.attach_ms(a, []()
							 { display.display(b); });
		}

		Serial.printf("a:%d, b:%d, brightness:%d\n", a, b, brightness);
	}
}
//****************Tools*********************
void DrawWeek(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t weekend_color, uint16_t bg)
{
	display.fillRect(x + 1, y, 6, 4, bg);
	display.fillRect(x + 9, y, 6, 4, bg);
	display.fillRect(x + 17, y, 6, 4, bg);
	display.fillRect(x + 25, y, 6, 4, bg);
	display.fillRect(x + 33, y, 6, 4, bg);
	display.fillRect(x + 41, y, 10, 4, bg);
	display.fillRect(x + 53, y, 10, 4, bg);
	switch (_week)
	{
	case 2:
		display.fillRect(x + 1, y, 6, 4, color);
		break;
	case 3:
		display.fillRect(x + 9, y, 6, 4, color);
		break;
	case 4:
		display.fillRect(x + 17, y, 6, 4, color);
		break;
	case 5:
		display.fillRect(x + 25, y, 6, 4, color);
		break;
	case 6:
		display.fillRect(x + 33, y, 6, 4, color);
		break;
	case 7:
		display.fillRect(x + 41, y, 10, 4, weekend_color);
		break;
	case 1:
		display.fillRect(x + 53, y, 10, 4, weekend_color);
		break;
	}
}
void MyPrintf(char *text, int x, int y, int _offsetX, int _offsetY, uint16_t color)
{
	int i = 0;
	while (i < strlen((char *)text))
	{
		if (text[i] > 127)
		{
			unsigned long offset = (94 * (unsigned int)(text[i] - 0xa0 - 1) + (text[i + 1] - 0xa0 - 1)) * 32;
			display.drawBitmap(i * 8 + x + _offsetX, y + _offsetY, &Hzk16s[offset], 16, 16, color, 0);
			i += 2;
		}
		else
		{
			unsigned long offset = text[i] * 16;
			display.drawBitmap(i * 8 + x + _offsetX, y + _offsetY, &Asc16[offset], 8, 16, color, 0);
			i++;
		}
	}
}
void SnakeGame_Callback()
{
	display.clearDisplay();
	unsigned long beforeTime = millis();
	Snake snake;
	snake.hdirection = "right";
	snake.keyDirection = "_S";
	snake.DisplaySnake();
	snake.CreatApple(snake.snakeLenX, snake.snakeLenY, snake.snakeLen);
	while (true)
	{
		if (snake.IsSnakeCollision(snake.hx, snake.hy))
		{
			return;
		}
		else
		{
			snake.DisplaySnake();
			if (snake.Key_Scan())
			{
				snake.DirectionAndCount();
				display.drawPixel(snake.snakeLenX[0], snake.snakeLenY[0], 0);
				snake.tx = snake.snakeLenX[0];
				snake.ty = snake.snakeLenY[0];
				snake.updateSnakePosition(false, snake.tx, snake.ty);
				beforeTime = millis();
			}
			if ((millis() - beforeTime) >= 100 * 3)
			{
				snake.AutoWalk();
				display.drawPixel(snake.snakeLenX[0], snake.snakeLenY[0], 0);
				snake.tx = snake.snakeLenX[0];
				snake.ty = snake.snakeLenY[0];
				snake.updateSnakePosition(false, snake.tx, snake.ty);
				beforeTime = millis();
			}
			if (snake.IfAppleEat(snake.hx, snake.hy, snake.appleX, snake.appleY))
			{
				snake.snakeLen++;
				display.drawPixel(snake.appleX, snake.appleY, 0);
				snake.updateSnakePosition(true, snake.tx, snake.ty);
				snake.CreatApple(snake.snakeLenX, snake.snakeLenY, snake.snakeLen);
				beforeTime = millis();
			}
		}
		delay(10);
	}
}
void httpRequest_Callback(bool is_rainbow_bg)
{
	WiFiClient client;
	String reqRes = String("") + "/v3/weather/daily.json?key=" + xinzhi_Key
					+ "&location=" + xinzhi_Location + "&language=zh-Hans&unit=" +
					xinzhi_Unit + "&start=0&days=3";
	// 建立http请求信息
	String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
						 "Host: " + host + "\r\n" +
						 "Connection: close\r\n\r\n";
	Serial.println("");
	Serial.print("Connecting to ");
	Serial.print(host);
	// 尝试连接服务器
	if (client.connect(host, 80))
	{
		Serial.println(" Success!");
		// 向服务器发送http请求信息
		client.print(httpRequest);
		Serial.println("Sending request: ");
		Serial.println(httpRequest);
		// 获取并显示服务器响应状态行
		String status_response = client.readStringUntil('\n');
		Serial.print("status_response: ");
		Serial.println(status_response);
		// 使用find跳过HTTP响应头
		if (client.find("\r\n\r\n"))
		{
			Serial.println("Found Header End. Start Parsing.");
		}
		// 利用ArduinoJson库解析心知天气响应信息

		parseWeather(client);
		Text_Callback((char *)stringBuffer.c_str(), is_rainbow_bg);
	}
	else
	{
		Serial.println(" connection failed!");
	}
	client.stop();
}
void parseWeather(WiFiClient client)
{
	const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 3 * JSON_OBJECT_SIZE(14) + 860;
	DynamicJsonDocument doc(capacity);
	deserializeJson(doc, client);
	JsonObject results_0 = doc["results"][0];
	JsonArray results_0_daily = results_0["daily"];
	JsonObject results_0_daily_0 = results_0_daily[0];
	// const char* results_0_daily_0_date = results_0_daily_0["date"];
	const char* results_0_daily_0_text_day = results_0_daily_0["text_day"];
	// const char* results_0_daily_0_code_day = results_0_daily_0["code_day"];
	const char* results_0_daily_0_text_night = results_0_daily_0["text_night"];
	// const char* results_0_daily_0_code_night = results_0_daily_0["code_night"];
	const char* results_0_daily_0_high = results_0_daily_0["high"];
	const char* results_0_daily_0_low = results_0_daily_0["low"];
	// const char* results_0_daily_0_rainfall = results_0_daily_0["rainfall"];
	// const char* results_0_daily_0_precip = results_0_daily_0["precip"];
	const char* results_0_daily_0_wind_direction = results_0_daily_0["wind_direction"];
	// const char* results_0_daily_0_wind_direction_degree = results_0_daily_0["wind_direction_degree"];
	// const char* results_0_daily_0_wind_speed = results_0_daily_0["wind_speed"];
	const char* results_0_daily_0_wind_scale = results_0_daily_0["wind_scale"];
	const char* results_0_daily_0_humidity = results_0_daily_0["humidity"];
	Serial.println("parse over");
	stringBuffer = String("") + 
					    "郑州今天白天" + results_0_daily_0_text_day +
					    "，夜里" + 	results_0_daily_0_text_night +
					    "，最高温度" + results_0_daily_0_high +
					    "℃，最低温度" + results_0_daily_0_low +
					    "℃，" + results_0_daily_0_wind_direction +
					    "风" + results_0_daily_0_wind_scale +
					    "级，湿度" + results_0_daily_0_humidity +
					    "％。";	

	// JsonObject results_0_daily_1 = results_0_daily[1];
	// const char *results_0_daily_1_date = results_0_daily_1["date"];
	// const char *results_0_daily_1_text_day = results_0_daily_1["text_day"];
	// const char *results_0_daily_1_code_day = results_0_daily_1["code_day"];
	// const char *results_0_daily_1_text_night = results_0_daily_1["text_night"];
	// const char *results_0_daily_1_code_night = results_0_daily_1["code_night"];
	// const char *results_0_daily_1_high = results_0_daily_1["high"];
	// const char *results_0_daily_1_low = results_0_daily_1["low"];
	// const char *results_0_daily_1_rainfall = results_0_daily_1["rainfall"];
	// const char *results_0_daily_1_precip = results_0_daily_1["precip"];
	// const char *results_0_daily_1_wind_direction = results_0_daily_1["wind_direction"];
	// const char *results_0_daily_1_wind_direction_degree = results_0_daily_1["wind_direction_degree"];
	// const char *results_0_daily_1_wind_speed = results_0_daily_1["wind_speed"];
	// const char *results_0_daily_1_wind_scale = results_0_daily_1["wind_scale"];
	// const char *results_0_daily_1_humidity = results_0_daily_1["humidity"];
	// JsonObject results_0_daily_2 = results_0_daily[2];
	// const char *results_0_daily_2_date = results_0_daily_2["date"];
	// const char *results_0_daily_2_text_day = results_0_daily_2["text_day"];
	// const char *results_0_daily_2_code_day = results_0_daily_2["code_day"];
	// const char *results_0_daily_2_text_night = results_0_daily_2["text_night"];
	// const char *results_0_daily_2_code_night = results_0_daily_2["code_night"];
	// const char *results_0_daily_2_high = results_0_daily_2["high"];
	// const char *results_0_daily_2_low = results_0_daily_2["low"];
	// const char *results_0_daily_2_rainfall = results_0_daily_2["rainfall"];
	// const char *results_0_daily_2_precip = results_0_daily_2["precip"];
	// const char *results_0_daily_2_wind_direction = results_0_daily_2["wind_direction"];
	// const char *results_0_daily_2_wind_direction_degree = results_0_daily_2["wind_direction_degree"];
	// const char *results_0_daily_2_wind_speed = results_0_daily_2["wind_speed"];
	// const char *results_0_daily_2_wind_scale = results_0_daily_2["wind_scale"];
	// const char *results_0_daily_2_humidity = results_0_daily_2["humidity"];
	// const char *results_0_last_update = results_0["last_update"];

}

 
