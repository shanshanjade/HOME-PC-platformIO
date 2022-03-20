#define PxMATRIX_COLOR_DEPTH 4
#define PxMATRIX_SPI_FREQUENCY 20000000
#include <Arduino.h>
#include <ArduinoJson.h>
#include <BmpDecoder.h>
#include <FS.h>
#include <Font_ApcFont.h>
#include <Font_Asc16.h>
#include <Font_Hzk16s.h>
#include <GifPlayer.h>
#include <NTPClient.h>
#include <PxMatrix.h>
#include <SPIFFS.h>
#include <Snake.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <config.h>
#include <UTF8toGB2312.c>

#define log(x) Serial.printf(x)

String last_Http_String = "";
// 心知天气
const char* xinzhi_Host = "api.seniverse.com";
const char* xinzhi_Key = "SN_WBgcbixJJwpJdi";  // 私钥
const char* xinzhi_Location = "郑州";          // 城市
const char* xinzhi_Unit = "c";                 // c摄氏
// 一言相关
const char* yiyan_Host = "v1.hitokoto.cn";
const char* yiyan_Parms = "/?c=a&c=b&c=b&c=d&c=e&c=f&c=g&c=h&c=i&c=j&c=k&c=l&c=其它";
// 聚合笑话
const char* juhe_Host = "v.juhe.cn";
const char* juhe_key = "5cb308eed5ec0ca7fc8a283028c8f96e";
String jokeString = "获取字符串的子字符串。     起始索引是包含的（对应的字符包含在子字符串中），     ";

// 解析图片所需要的 解析器
GifPlayer gifPlayer;
BmpDecoder myBmpDecoder;
// 显示函数相关
uint8_t display_draw_time = 15;  //30-70 is usually fine // display_draw_time:10 brightness:128
String control_String = "ih";
bool is_mode_changed_flag = false;

//NTP获取网络时间相关
WiFiUDP Udp;
NTPClient timeClient(Udp, "ntp1.aliyun.com");

// 文件系统相关
String file_name;
File myFile;
File root;

// 函数声明
void clock_Date_Callback();
void image_Play_Callback();
void text_Scroll_Callback(String myString, uint8_t scroll_delay, uint16_t color, uint16_t bgcolor, bool is_rainbow_bg);
void game_Snake_Callback();
void draw_Week(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t weekend_color, uint16_t bg);
void my_Printf(char* text, int x, int y, int _offsetX, int _offsetY, uint16_t color);
String read_Text_SPIFFS(char* filePath, bool auto_next_line);

String httpRequest_Weather_Callback();
String httpRequest_Hitokoto_Callback();
String httpRequest_JuheJoke_Callback();
String parse_Weather(String clientString);
String parse_Hitokoto(String clientString);
String parse_JuheJoke(String clientString);

void displayTask(void* displayTask) {
    while (1) {
        display.display(display_draw_time);
        vTaskDelay(1);  //此处是延时1ms
    }
}
void serialListenTask(void* serialListen) {
    while (1) {
        if (Serial.available() > 0) {
            Serial.printf("Received control string!\n");
            control_String = Serial.readString();

            File configFile = SPIFFS.open("/config/config.txt", "w");
            configFile.print(control_String);
            configFile.close();

            is_mode_changed_flag = true;
        }
        vTaskDelay(500);
    }
}
// void myloopTask(void * myloopTask)
// {
// 	while (1)
// 	{
// 	}
// }
TaskHandle_t taskHandle_display;  //display的任务句柄，句柄用来操作线程
void init_Display() {
    display.begin(16);
    display.setBrightness(128);
    display.setColorOffset(0, 0, 100);  //best (0, 0, 110)
    display.setTextColor(0x07E0);
    display.setTextWrap(false);
    display.setFastUpdate(true);
    display.clearDisplay();
    xTaskCreatePinnedToCore(displayTask, "displayTask", 1000, NULL, 2, &taskHandle_display, 1);  // PRO_CPU 为 0, APP_CPU 为 1 4096
};
void init_WiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin("xiaomi123", "a.13513710972");
    my_Printf("WiFi", 0, 0, (64 - 4 * 8) / 2, 0, 0xF800);
    int a = 0;
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(20);
        my_Printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>", -28 * 8, 16, a++, 0, 0x07E0);
    }
    display.fillScreen(0);
    my_Printf("WiFi", 0, 0, (64 - 4 * 8) / 2, 0, 0x07E0);
    my_Printf("DONE!", 0, 16, (64 - 5 * 8) / 2, 0, 0x07E0);
    vTaskDelay(2000);
    my_Printf("ENJOY:)", 0, 16, (64 - 7 * 8) / 2, 0, 0xF81E);
    vTaskDelay(2000);
};
void init_timeClient() {
    display.clearDisplay();
    timeClient.begin();
    timeClient.setTimeOffset(8 * 60 * 60);  //时区设置8，单位是秒
    timeClient.setUpdateInterval(60);       //自动校准时间的时间间隔，单位是秒
    timeClient.update();
}
void init_SPIFFS() {
    SPIFFS.begin();
    root = SPIFFS.open("/");
    myFile = root.openNextFile();
    file_name = myFile.name();
    Serial.printf("SPIFFS文件系统大小为：%d，已使用：%d，使用率%d%% \n ", SPIFFS.totalBytes(), SPIFFS.usedBytes(), SPIFFS.usedBytes() * 100 / SPIFFS.totalBytes());
}
void init_Config() {
    File configFile;
    configFile = SPIFFS.open("/config/config.txt", "r");
    control_String = configFile.readString();
    configFile.close();
}
void setup() {
    Serial.begin(115200);
    xTaskCreatePinnedToCore(serialListenTask, "serialListenTask", 2000, NULL, 1, NULL, 1);  // PRO_CPU 为 0, APP_CPU 为 1
    init_Display();
    init_WiFi();
    init_timeClient();
    init_SPIFFS();
    init_Config();
    Serial.printf("Help: c:clock, w:weather, h:hitokoto, j:joke, i:image, g:game, t:text\n");
    // xTaskCreatePinnedToCore(myloopTask, "myloopTask", 4096*20, NULL, 100, NULL, 1); // PRO_CPU 为 0, APP_CPU 为 1
}

void loop() {
    static u8_t i = 0;
    char* p = (char*)control_String.c_str();
    switch (p[i]) {
        case 'c':
            clock_Date_Callback();
            break;
        case 'w':
            text_Scroll_Callback(httpRequest_Weather_Callback(), 25, display.colorHSV(random(255), 255, 255), 0, false);
            break;
        case 'h':
            text_Scroll_Callback(httpRequest_Hitokoto_Callback(), 25, display.colorHSV(random(255), 255, 255), 0, false);
            break;
        case 'j':
            text_Scroll_Callback(httpRequest_JuheJoke_Callback(), 25, display.colorHSV(random(255), 255, 255), 0, false);
            break;
        case 'i':
            image_Play_Callback();
            break;
        case 'g':
            game_Snake_Callback();
            break;
        case 't':
            text_Scroll_Callback(read_Text_SPIFFS("/text/daodejing.txt", true), 25, display.colorHSV(random(255), 255, 255), 0, false);
            break;
        default:
            break;
    }
    if (++i > control_String.length() - 1)
        i = 0;
    timeClient.update();
    Serial.printf("Current Control String: %s\n", control_String.c_str());
    Serial.printf("%lds: Free heap:%d\n", millis() / 1000, ESP.getFreeHeap());
}

void image_Play_Callback() {
    display.clearDisplay();
    myFile = root.openNextFile("r");
    file_name = myFile.name();
    if (file_name.length() == 0 || myFile.isDirectory())
        root.rewindDirectory();

    static uint8_t mode_index = 1;
    File imageFile = myFile;
    if (file_name.endsWith(".gif") || file_name.endsWith(".GIF"))
        mode_index = 0;
    else if (file_name.endsWith(".bmp") || file_name.endsWith(".BMP"))
        mode_index = 1;
    else
        return;
    if (mode_index == 0) {
        // Serial.println("gif mode");
        unsigned long time_point = millis();
        while ((millis() - time_point) <= 8000) {
            if (is_mode_changed_flag) {
                is_mode_changed_flag = false;
                return;
            }
            uint32_t result;
            if (!imageFile) {
                Serial.println(F("Failed to open"));
                return;
            }
            gifPlayer.setFile(imageFile);
            if (!gifPlayer.parseGifHeader()) {
                imageFile.close();
                Serial.println(F("No gif header"));
                return;
            }
            gifPlayer.parseLogicalScreenDescriptor();
            gifPlayer.parseGlobalColorTable();
            vTaskDelay(10);
            do {
                gifPlayer.drawFrame();
                result = gifPlayer.drawFrame();
            } while (result != ERROR_FINISHED);
            imageFile.seek(0);
        }
    } else if (mode_index == 1) {
        // Serial.println("bmp mode");
        myBmpDecoder.setFile(imageFile);
        unsigned long time_point = millis();
        int bmp_scroll_offset = 0;
        int direction = -1;
        while (millis() - time_point <= 15000) {
            if (myBmpDecoder.G_biHeight <= 32) {
                display.drawRGBBitmap((64 - myBmpDecoder.G_biWidth) / 2, (32 - myBmpDecoder.G_biHeight) / 2, myBmpDecoder.BitMap_buffer, myBmpDecoder.G_biWidth, myBmpDecoder.G_biHeight);
            } else {
                display.drawRGBBitmap((64 - myBmpDecoder.G_biWidth) / 2, 0 + bmp_scroll_offset, myBmpDecoder.BitMap_buffer, myBmpDecoder.G_biWidth, myBmpDecoder.G_biHeight);
            }
            bmp_scroll_offset = bmp_scroll_offset + direction;
            if (bmp_scroll_offset <= 32 - myBmpDecoder.G_biHeight)
                direction = 1;
            else if (bmp_scroll_offset >= 0)
                direction = -1;
            vTaskDelay(15);
        }
        myBmpDecoder.free();
    }
}
void text_Scroll_Callback(String myString, uint8_t scroll_delay, uint16_t color, uint16_t bgcolor, bool is_rainbow_bg) {
    display.clearDisplay();
    char myword[800];
    GetGb2312String((char*)myString.c_str(), myword);
    GFXcanvas1 myCanvas1_text(64, 16);
    GFXcanvas16 myCanvas16_text(64, 16);
    int text_scroll_offset = 0;
    int myword_len = strlen((char*)myword);
    int myword_width = strlen((char*)myword) * 8 + 64 - 1;
    //读取点阵字库并 滚动显示
    while (text_scroll_offset <= myword_width) {
        if (is_mode_changed_flag) {
            is_mode_changed_flag = false;
            return;
        }
        unsigned long time_point = millis();
        int i = 0;
        while (i < myword_len) {
            if (myword[i] > 127) {
                unsigned long offset = (94 * (unsigned int)(myword[i] - 0xa0 - 1) + (myword[i + 1] - 0xa0 - 1)) * 32;
                myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &Hzk16s[offset], 16, 16, 0xF800, 0);
                i += 2;
            } else {
                unsigned long offset = myword[i] * 16;
                myCanvas1_text.drawBitmap(i * 8 - text_scroll_offset + 64, 0, &Asc16[offset], 8, 16, 0x07E0, 0);
                i++;
            }
        }
        text_scroll_offset++;
        // display.fillRect(0, 0, 64, 32, 0);
        if (is_rainbow_bg) {
            for (int i = 0; i < 64 + 16; i++) {
                myCanvas16_text.drawLine(i, 0, -16 + i, 16, display.colorHSV((i + text_scroll_offset) * 3, 255, 255));
            }
            display.drawRGBBitmap(0, 8, myCanvas16_text.getBuffer(), 64, 16);
            display.drawBitmapInvert(0, 8, myCanvas1_text.getBuffer(), 64, 16, 0);
        } else {
            display.drawBitmap(0, 8, myCanvas1_text.getBuffer(), 64, 16, color, bgcolor);
        }
        uint16_t temp = millis() - time_point;
        if (temp <= scroll_delay)
            temp = scroll_delay - temp;
        else
            temp = 1;
        vTaskDelay(temp);
    }
}
void clock_Date_Callback() {
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    // Serial.println(epochTime);
    struct tm* p_tm = gmtime((time_t*)&epochTime);
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
    my_Printf((char*)formattedDate.c_str(), 0, -1, 0, 0, 0x03E0);
    draw_Week(timeClient.getDay() + 1, 0, 29, 0x07FF, 0xf800, display.color565(40, 40, 150));
    while (millis() - time_point <= 10000) {
        if (is_mode_changed_flag) {
            is_mode_changed_flag = false;
            return;
        }
        my_Printf((char*)timeClient.getFormattedTime().c_str(), 0, 13, 0, 0, 0xD100);
        vTaskDelay(50);
    }
}

//****************Tools*********************
String read_Text_SPIFFS(char* filePath, bool auto_next_line) {
    String temp;
    static uint32_t file_position = 0;
    File myTextFile = SPIFFS.open(filePath, "r");
    if (myTextFile) {
        if (auto_next_line) {
            myTextFile.seek(file_position);           //将文件指针跳至指定位置
            temp = myTextFile.readStringUntil('\n');  //读取文件中的字符串直到换行符, 但换行符也会被读取进去,后面需要进行处理
            file_position = myTextFile.position();    //变量记录当前的文件指针位置
            temp.setCharAt(temp.length() - 1, '\0');  //去掉换行符，否则末尾会乱码
        } else {
            temp = myTextFile.readString();
        }
    } else {
        temp = "未找到文件！";
    }
    if (temp == "")
    {
        file_position = 0;
        return "END OF FILE！";
    } else {
        return temp;
    }
    
}
void draw_Week(uint8_t _week, uint8_t x, uint8_t y, uint16_t color, uint16_t weekend_color, uint16_t bg) {
    display.fillRect(x + 1, y, 6, 4, bg);
    display.fillRect(x + 9, y, 6, 4, bg);
    display.fillRect(x + 17, y, 6, 4, bg);
    display.fillRect(x + 25, y, 6, 4, bg);
    display.fillRect(x + 33, y, 6, 4, bg);
    display.fillRect(x + 41, y, 10, 4, bg);
    display.fillRect(x + 53, y, 10, 4, bg);
    switch (_week) {
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
void my_Printf(char* text, int x, int y, int _offsetX, int _offsetY, uint16_t color) {
    int i = 0;
    while (i < strlen((char*)text)) {
        if (text[i] > 127) {
            unsigned long offset = (94 * (unsigned int)(text[i] - 0xa0 - 1) + (text[i + 1] - 0xa0 - 1)) * 32;
            display.drawBitmap(i * 8 + x + _offsetX, y + _offsetY, &Hzk16s[offset], 16, 16, color, 0);
            i += 2;
        } else {
            unsigned long offset = text[i] * 16;
            display.drawBitmap(i * 8 + x + _offsetX, y + _offsetY, &Asc16[offset], 8, 16, color, 0);
            i++;
        }
    }
}
void game_Snake_Callback() {
    display.clearDisplay();
    unsigned long beforeTime = millis();
    Snake snake;
    snake.hdirection = "right";
    snake.keyDirection = "_S";
    snake.DisplaySnake();
    snake.CreatApple(snake.snakeLenX, snake.snakeLenY, snake.snakeLen);
    while (true) {
        if (is_mode_changed_flag) {
            is_mode_changed_flag = false;
            return;
        }
        if (snake.IsSnakeCollision(snake.hx, snake.hy)) {
            return;
        } else {
            snake.DisplaySnake();
            if (snake.Key_Scan()) {
                snake.DirectionAndCount();
                display.drawPixel(snake.snakeLenX[0], snake.snakeLenY[0], 0);
                snake.tx = snake.snakeLenX[0];
                snake.ty = snake.snakeLenY[0];
                snake.updateSnakePosition(false, snake.tx, snake.ty);
                beforeTime = millis();
            }
            if ((millis() - beforeTime) >= 100 * 3) {
                snake.AutoWalk();
                display.drawPixel(snake.snakeLenX[0], snake.snakeLenY[0], 0);
                snake.tx = snake.snakeLenX[0];
                snake.ty = snake.snakeLenY[0];
                snake.updateSnakePosition(false, snake.tx, snake.ty);
                beforeTime = millis();
            }
            if (snake.IfAppleEat(snake.hx, snake.hy, snake.appleX, snake.appleY)) {
                snake.snakeLen++;
                display.drawPixel(snake.appleX, snake.appleY, 0);
                snake.updateSnakePosition(true, snake.tx, snake.ty);
                snake.CreatApple(snake.snakeLenX, snake.snakeLenY, snake.snakeLen);
                beforeTime = millis();
            }
        }
        vTaskDelay(10);
    }
}
String httpRequest_Hitokoto_Callback() {
    WiFiClient client;
    String temp;
    String httpRequest = String("GET ") + yiyan_Parms + " HTTP/1.1\r\n" +
                         "Host: " + yiyan_Host + "\r\n" +
                         "Connection: close\r\n\r\n";
    if (client.connect(yiyan_Host, 80)) {
        //发送请求
        client.print(httpRequest);
        //等待接收数据并判断超时
        unsigned long timeout = millis();
        while (client.connected() && !client.available()) {
            if (millis() - timeout > 5000) {
                client.stop();
                return String("Client Timeout!");
            }
        }
        //跳过响应头
        if (client.find("\r\n\r\n"))
            temp = parse_Hitokoto(client.readString());  //找到响应头
        else
            temp = String("Cannot Found Header End.");  //没有找到响应头
    } else {
        temp = String("Connection Failed!");
    }
    client.stop();
    return temp;
}
String parse_Hitokoto(String clientString) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, clientString);
    if (error) {
        Serial.print("deserializeJson() failed: hitokoto");
        Serial.println(error.c_str());
        return String("deserializeJson() failed: hitokoto");
    }
    const char* hitokoto = doc["hitokoto"];
    const char* from = doc["from"];
    const char* from_who = doc["from_who"];

    if (from_who == NULL)
        from_who = "unknown";  //没有此句 程序会崩溃

    last_Http_String = String("") + hitokoto + " From:" + from + "  By:" + from_who;
    return last_Http_String;
}
String httpRequest_Weather_Callback() {
    WiFiClient client;
    String temp;
    String reqRes = String("") + "/v3/weather/daily.json?key=" + xinzhi_Key + "&location=" + xinzhi_Location + "&language=zh-Hans&unit=" +
                    xinzhi_Unit + "&start=0&days=3";
    // 建立http请求信息
    String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                         "Host: " + xinzhi_Host + "\r\n" +
                         "Connection: close\r\n\r\n";
    if (client.connect(xinzhi_Host, 80)) {
        // 向服务器发送http请求信息
        client.print(httpRequest);
        //等待接收数据并判断超时
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                client.stop();
                return String("Client Timeout !");
            }
        }
        // 使用find跳过HTTP响应头
        if (client.find("\r\n\r\n")) {
            temp = parse_Weather(client.readString());
        }
    } else {
        temp = String("connection failed!");
    }
    client.stop();
    return temp;
}
String parse_Weather(String clientString) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, clientString);
    if (error) {
        return String("deserializeJson() failed: weather");
    }
    JsonObject results_0 = doc["results"][0];
    JsonArray results_0_daily = results_0["daily"];
    JsonObject results_0_daily_0 = results_0_daily[0];
    const char* results_0_daily_0_text_day = results_0_daily_0["text_day"];
    const char* results_0_daily_0_text_night = results_0_daily_0["text_night"];
    const char* results_0_daily_0_high = results_0_daily_0["high"];
    const char* results_0_daily_0_low = results_0_daily_0["low"];
    const char* results_0_daily_0_wind_direction = results_0_daily_0["wind_direction"];
    const char* results_0_daily_0_wind_scale = results_0_daily_0["wind_scale"];
    const char* results_0_daily_0_humidity = results_0_daily_0["humidity"];
    last_Http_String = String("") + xinzhi_Location +
                       "今天白天" + results_0_daily_0_text_day +
                       "，夜里" + results_0_daily_0_text_night +
                       "，最高温度" + results_0_daily_0_high +
                       "℃，最低温度" + results_0_daily_0_low +
                       "℃，" + results_0_daily_0_wind_direction +
                       "风" + results_0_daily_0_wind_scale +
                       "级，湿度" + results_0_daily_0_humidity +
                       "％。";
    return last_Http_String;
}
String httpRequest_JuheJoke_Callback() {
    String temp;
    if (jokeString.length() == 0) {
        WiFiClient client;
        String httpRequest = String("GET ") + "/joke/randJoke.php?key=" + juhe_key + " HTTP/1.1\r\n" +
                             "Host: " + juhe_Host + "\r\n" +
                             "Connection: close\r\n\r\n";
        if (client.connect(juhe_Host, 80)) {
            //发送请求
            client.print(httpRequest);
            //等待接收数据并判断超时
            unsigned long timeout = millis();
            while (client.connected() && !client.available()) {
                if (millis() - timeout > 5000) {
                    client.stop();
                    return String("Client Timeout!");
                }
            }
            //跳过响应头
            if (client.find("\r\n\r\n"))
                temp = parse_JuheJoke(client.readString());  //找到响应头 ,将10个笑话存到jokeString中
            else
                temp = String("Cannot Found Header End.");  //没有找到响应头
        } else {
            temp = String("Connection Failed!");
        }
        client.stop();
    } 

    temp = jokeString.substring(0, jokeString.indexOf("     ") + 5);     //从头截到索引位置 得到一条笑话
    jokeString = jokeString.substring(jokeString.indexOf("     ") + 5);  //从索引位置截到尾部（jokeString删除第一条joke）
    Serial.printf("Current jokeString: %s\n", jokeString.c_str());

    return temp;
}
String parse_JuheJoke(String clientString) {
    clientString = clientString.substring(clientString.indexOf('{'), clientString.lastIndexOf('}')+1);  //清洗字符串
    Serial.println(clientString);
    DynamicJsonDocument doc(9000);
    DeserializationError error = deserializeJson(doc, clientString);
    if (error) {
        Serial.print("deserializeJson() failed: juhe_joke");
        Serial.println(error.c_str());
        return String("deserializeJson() failed: juhe_joke");
    }
    jokeString = "";
    const char* reason = doc["reason"];  // "success"

    if (String(reason) == "success") {
        for (JsonObject result_item : doc["result"].as<JsonArray>()) {
            const char* result_item_content = result_item["content"];
            jokeString = jokeString + result_item_content + "     ";
            // const char* result_item_hashId = result_item["hashId"];  // "348e7f933774bcaef3ed3f0ecb8e7b19", ...
            // long result_item_unixtime = result_item["unixtime"];     // 1418819032, 1418819032, 1418819032, 1418819032, ...
        }
        // int error_code = doc["error_code"];  // 0
        return jokeString;
    } 
    else {
        return String(reason);
    }
}