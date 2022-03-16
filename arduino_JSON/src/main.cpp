#include <FastLED.h>
#include <Ticker.h>
#include <Wire.h> //DS3231使用
#include <DS3231.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>

#define NUM_LEDS 120
#define DATA_LEDS_PIN D3
#define LED_TYPE WS2812
#define COLOR_ORDER GRB

#define BUTTON_PIN D7

#define SSID_ADDR 31
#define PSWD_ADDR 1
#define BOOT_MODE_ADDR 0
#define E_H12_ADDR 61
#define TIMING_ON 62
#define OPEN_TIME 63
#define CLOSE_TIME 69

struct
{
    char ssid[30];  
    char pswd[30];  
    bool boot_mode; //0-正常模式，1-离线+配网模式
    bool E_h12;
    bool timing_on;
    char open_time[6];
    char close_time[6];
} eeprom_save;



WiFiUDP Udp;
ESP8266WebServer server(80);

CRGB leds[NUM_LEDS], myrgbColor(255,0,0);
CHSV myhsvColor(0,255,255), 
     myhsvColor_h1(0,255,255),myhsvColor_h2(0,255,255), myhsvColor_m1(0,255,255),myhsvColor_m2(0,255,255), myhsvColor_s1(0,255,255),myhsvColor_s2(0,255,255);
//时钟相关
DS3231 Clock;
bool h12;
bool AM;
bool Century;
//时间日期变量
uint8_t c_hours = 21,c_minutes = 17,c_seconds = 20,c_years = 20,c_months = 7,c_days = 30;
//温度变量
int8_t  c_temperatures = 28;
//时间每位的数值
uint8_t c_hours_1,c_hours_2, c_minutes_1,c_minutes_2, c_seconds_1,c_seconds_2;
//翻页动画相关
bool c_hours_1_flag = 1,c_hours_2_flag = 1, c_minutes_1_flag = 1,c_minutes_2_flag = 1, c_seconds_1_flag = 1,c_seconds_2_flag = 1;
//模式相关变量
bool timing_Task_once1 = true;
bool timing_Task_once2 = true;

uint8_t mode_flag = 2;
uint8_t last_mode_flag = 1;
uint8_t display_flag = 0;
uint8_t turnPage_flag = 3;
uint8_t jumpTimer = 0;
uint8_t jumpTimer_temp = 0;
int8_t temperature_i = 0;
int8_t breath_flag = 1;
int8_t speed = 5;
//开关
bool isOn = true;
//6个灯位中每个灯位的开关
bool switch_value[6]={0,0,0,0,0,0};
uint8_t color[3]={0,0,0};

//Ticker相关函数
Ticker timer_Ticker;
Ticker FastLED_show_Ticker;
void timer_ticker_callback();
void FastLED_show_ticker_callback();

//显示内容相关函数
void display_time();
void display_date();
void display_temperature();
void display_light();
void display_pressTime();
//显示模式设置
void set_mode();
void set_turnPage();
void set_display();

void check_every_num_ainamation();
void check_UDP_recived_parse();
void timing_on_task();
//按键中断函数
ICACHE_RAM_ATTR void buttonInterrupt();
bool keydown = false;
bool keyup = true;
uint16_t pressTime = 0;
//webServer 相关
void handleRootRequest();
const char* webpage_html = "\
<!DOCTYPE html>\r\n\
<html lang='en'>\r\n\
<head>\r\n\
  <meta charset='UTF-8'>\r\n\
  <title>Document</title>\r\n\
</head>\r\n\
<body>\r\n\
  <form name='input' action='/' method='POST'>\r\n\
        wifi名称: <br>\r\n\
        <input type='text' name='ssid'><br>\r\n\
        wifi密码:<br>\r\n\
        <input type='text' name='pswd'><br>\r\n\
        <input type='submit' value='保存'>\r\n\
    </form>\r\n\
</body>\r\n\
</html>\r\n\
";

void setup(){
    Serial.begin(115200);
    Wire.begin();
    EEPROM.begin(1024);
    EEPROM.get(0, eeprom_save);
    EEPROM.end();

    Serial.println(eeprom_save.ssid);
    Serial.println(eeprom_save.pswd);
    Serial.println(eeprom_save.boot_mode?"离线模式":"正常模式");
    Serial.println(eeprom_save.E_h12?"12小时制":"24小时制");
    Serial.println(eeprom_save.timing_on?"已开启定时任务":"未开启定时任务");
    if (eeprom_save.timing_on)
    {
        Serial.printf("开启时间：%s\n", eeprom_save.open_time);
        Serial.printf("关闭时间：%s\n", eeprom_save.close_time);
    }

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, CHANGE);

    c_hours  = Clock.getHour(h12,AM);
    c_minutes= Clock.getMinute();
    c_seconds= Clock.getSecond();
    c_years  = Clock.getYear();
    c_months = Clock.getMonth(Century);
    c_days   = Clock.getDate();
    c_temperatures = Clock.getTemperature();

    LEDS.addLeds<LED_TYPE, DATA_LEDS_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(30);
    FastLED.showColor(CRGB::Black);

    timer_Ticker.attach(1,timer_ticker_callback);
    FastLED_show_Ticker.attach_ms(10, FastLED_show_ticker_callback);
    
    unsigned long last_millis = millis();
    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);

    check_every_num_ainamation();

    if (eeprom_save.boot_mode == 0)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(eeprom_save.ssid, eeprom_save.pswd);
        while (WiFi.status() != WL_CONNECTED && (millis()-last_millis)<=20000) {
            delay(500);
            Serial.print(".");
            check_every_num_ainamation();
        }
        if (WiFi.status() != WL_CONNECTED){
            Serial.println("\n未能连接上WIFI\n将进入离线+配网模式");
            eeprom_save.boot_mode = 1;
            EEPROM.begin(1024);
            EEPROM.put(0, eeprom_save);
            EEPROM.end();
            delay(100);
            ESP.restart();
        } else {
            Serial.println("已连接上wifi！");
            Serial.println(WiFi.localIP());
        }
    } 
    else 
    {
        Serial.println("进入离线+配网模式");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(local_IP,gateway,subnet);
        WiFi.softAP("RGB辉光时钟配网");
        server.on("/", HTTP_GET, handleRootRequest);
        server.on("/", HTTP_POST, handleRootRequest);
        server.begin();
        Serial.println("HTTP服务器已启动");
        Serial.println(WiFi.softAPIP());
    }
    
    Udp.begin(8888);
    MDNS.begin("esp8266");
    MDNS.addService("biubiu","udp",8888);
}

void loop(){
    if (eeprom_save.boot_mode == 0){
        check_UDP_recived_parse();
    } else {
        server.handleClient();
    }

    FastLED.clear();
    if(keydown == true){
        pressTime++;
        display_pressTime();
    } else {
        if(isOn){
            set_display();
            set_mode();
            set_turnPage();
        }else{
            fill_solid(leds,NUM_LEDS,CRGB::Black);
        }
        if(pressTime != 0 ){
            if(pressTime <= 10){} //消除抖动
            else if(pressTime <= 100){
                isOn = !isOn;
            }
            else if(pressTime <= 200){
                mode_flag++; if(mode_flag > 5)mode_flag = 0;
                FastLED.showColor(CRGB::Blue);
            }
            else if(pressTime <= 300){
                display_flag++; if(display_flag > 2){display_flag = 0;turnPage_flag =3;};  
                FastLED.showColor(CRGB::Green);
            }
            else if(pressTime <= 400){
                turnPage_flag++; if(turnPage_flag > 4)turnPage_flag = 0; 
                FastLED.showColor(CRGB::Orange);
            }
            else if(pressTime >=1000 && pressTime < 1100 ){
                FastLED.showColor(CRGB::Red);
                delay(200);
                eeprom_save.boot_mode = 0;
                EEPROM.begin(1024);
                EEPROM.put(0, eeprom_save);
                EEPROM.end();
                delay(100);
                ESP.reset();
            }
            else if(pressTime >= 1500 && pressTime < 1600){
                FastLED.showColor(CRGB(0,255,255));
                delay(200);
                eeprom_save.boot_mode = 1;
                EEPROM.begin(1024);
                EEPROM.put(0, eeprom_save);
                EEPROM.end();
                delay(100);
                ESP.restart();
            }
            pressTime = 0;
            delay(200);
        }
    }
    delay(10);
}

void display_time(){
    leds[2*(c_hours_1)       ] = myhsvColor_h1; leds[2*(c_hours_1)   + 1 ] = myhsvColor_h1;
    leds[2*(c_hours_2)   + 20] = myhsvColor_h2; leds[2*(c_hours_2)   + 21] = myhsvColor_h2;
    leds[2*(c_minutes_1) + 40] = myhsvColor_m1; leds[2*(c_minutes_1) + 41] = myhsvColor_m1;
    leds[2*(c_minutes_2) + 60] = myhsvColor_m2; leds[2*(c_minutes_2) + 61] = myhsvColor_m2;
    leds[2*(c_seconds_1) + 80] = myhsvColor_s1; leds[2*(c_seconds_1) + 81] = myhsvColor_s1;
    leds[2*(c_seconds_2) +100] = myhsvColor_s2; leds[2*(c_seconds_2) +101] = myhsvColor_s2;
}
void display_date(){
    turnPage_flag = 4;//关闭翻页动画
    leds[2*(c_years/10)       ] = myhsvColor_h1; leds[2*(c_years/10)   + 1 ] = myhsvColor_h1;
    leds[2*(c_years%10)   + 20] = myhsvColor_h2; leds[2*(c_years%10)   + 21] = myhsvColor_h2;
    leds[2*(c_months/10) + 40] = myhsvColor_m1; leds[2*(c_months/10) + 41] = myhsvColor_m1;
    leds[2*(c_months%10) + 60] = myhsvColor_m2; leds[2*(c_months%10) + 61] = myhsvColor_m2;
    leds[2*(c_days/10) + 80] = myhsvColor_s1; leds[2*(c_days/10) + 81] = myhsvColor_s1;
    leds[2*(c_days%10) +100] = myhsvColor_s2; leds[2*(c_days%10) +101] = myhsvColor_s2;
}
void display_temperature(){
    turnPage_flag = 4;//关闭翻页动画
    if(abs(c_temperatures)/10 > 0){leds[2*(abs(c_temperatures)/10) + 40] = myhsvColor_m1; leds[2*(abs(c_temperatures)/10) + 41] = myhsvColor_m1;}
    leds[2*(abs(c_temperatures)%10) + 60] = myhsvColor_m2; leds[2*(abs(c_temperatures)%10) + 61] = myhsvColor_m2;
    if(c_temperatures >= 0){ //温度是零上多少度
        leds[2*temperature_i +  0] = myhsvColor_h1;  leds[2*temperature_i +  1] = myhsvColor_h1;
        leds[2*temperature_i + 20] = myhsvColor_h2;  leds[2*temperature_i + 21] = myhsvColor_h2;
        leds[2*temperature_i + 80] = myhsvColor_s1;  leds[2*temperature_i + 81] = myhsvColor_s1;
        leds[2*temperature_i +100] = myhsvColor_s2;  leds[2*temperature_i +101] = myhsvColor_s2;
        if(jumpTimer_temp == 3){
            temperature_i++;
            if(temperature_i > 9)temperature_i = 0;
            jumpTimer_temp = 0;
        }
        jumpTimer_temp++;
    }else{              //温度是零下多少度
        leds[2*temperature_i +  0] = myhsvColor_h1;  leds[2*temperature_i +  1] = myhsvColor_h1;
        leds[2*temperature_i + 20] = myhsvColor_h2;  leds[2*temperature_i + 21] = myhsvColor_h2;
        leds[2*temperature_i + 80] = myhsvColor_s1;  leds[2*temperature_i + 81] = myhsvColor_s1;
        leds[2*temperature_i +100] = myhsvColor_s2;  leds[2*temperature_i +101] = myhsvColor_s2;
        if(jumpTimer_temp == 3){
            temperature_i--;
            if(temperature_i < 0)temperature_i = 9;
            jumpTimer_temp = 0;
        }
        jumpTimer_temp++;
    }
}
void display_light(){
    turnPage_flag = 4; //关闭翻页动画
    for(uint8_t i = 0; i < 10; i++ ){
        leds[2*i     ] = myhsvColor_h1; leds[2*i + 1 ] = myhsvColor_h1;
        leds[2*i + 20] = myhsvColor_h2; leds[2*i + 21] = myhsvColor_h2;
        leds[2*i + 40] = myhsvColor_m1; leds[2*i + 41] = myhsvColor_m1;
        leds[2*i + 60] = myhsvColor_m2; leds[2*i + 61] = myhsvColor_m2;
        leds[2*i + 80] = myhsvColor_s1; leds[2*i + 81] = myhsvColor_s1;
        leds[2*i +100] = myhsvColor_s2; leds[2*i +101] = myhsvColor_s2;
    }
}
void display_pressTime(){
	c_hours_1 = c_hours_2 = 0;
	c_seconds_2 = pressTime%10;
	c_seconds_1 = pressTime/10%10;
	c_minutes_2 = pressTime/100%10;
    c_minutes_1 = pressTime/1000%10;

	// leds[2*(c_hours_1)       ] = myhsvColor_h1; leds[2*(c_hours_1)   + 1 ] = myhsvColor_h1;
	// leds[2*(c_hours_2)   + 20] = myhsvColor_h2; leds[2*(c_hours_2)   + 21] = myhsvColor_h2;
	leds[2*(c_minutes_1) + 40] = myhsvColor_m1; leds[2*(c_minutes_1) + 41] = myhsvColor_m1;
	leds[2*(c_minutes_2) + 60] = myhsvColor_m2; leds[2*(c_minutes_2) + 61] = myhsvColor_m2;
	// leds[2*(c_seconds_1) + 80] = myhsvColor_s1; leds[2*(c_seconds_1) + 81] = myhsvColor_s1;
	// leds[2*(c_seconds_2) +100] = myhsvColor_s2; leds[2*(c_seconds_2) +101] = myhsvColor_s2;


}

void set_display(){
    switch(display_flag){
            case 0 :
                display_time();
                break;
            case 1 :
                display_date();
                break;
            case 2 :
                display_temperature();
                break;
            case 3 :
                display_light();
                break;
    }
}
void set_mode(){
    //给定初始颜色，只执行一次 每次mode——flag变化，以下内容便执行一次
    if(last_mode_flag != mode_flag  ){
        if     (mode_flag == 0) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(random8(),255,255);
        else if(mode_flag == 1) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(random8(),255,255);
        else if(mode_flag == 2) {myhsvColor_h1.h+=0;myhsvColor_h2.h+=17;myhsvColor_m1.h+=34;myhsvColor_m2.h+=51;myhsvColor_s1.h+=68;myhsvColor_s2.h+=85;}
        else if(mode_flag == 3) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(random8(),255,255);
        else if(mode_flag == 4) {myhsvColor_h1.h+=0;myhsvColor_h2.h+=17;myhsvColor_m1.h+=34;myhsvColor_m2.h+=51;myhsvColor_s1.h+=68;myhsvColor_s2.h+=85;}
        else if(mode_flag == 5) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(random8(),255,255);
        last_mode_flag = mode_flag;
    }

    switch(mode_flag){
        case 0:     //同步流光，需要给初始颜色(6个全部一样)，点击无效/关灯
            if(jumpTimer >= speed/2){
                myhsvColor_h1.h+=2;    
                myhsvColor_h2.h+=2;
                myhsvColor_m1.h+=2;
                myhsvColor_m2.h+=2;
                myhsvColor_s1.h+=2;
                myhsvColor_s2.h+=2;  
                jumpTimer = 0;
            }
            jumpTimer++;
            break;
        case 1:     //单色，需要给初始颜色(6个全部一样)，点击变色
            break;
        case 2:     //渐变流光，需要给初始颜色(6个全部一样)，点击变色
            if(jumpTimer >= speed/2){
                myhsvColor_h1.h+=2;    
                myhsvColor_h2.h+=2;
                myhsvColor_m1.h+=2;
                myhsvColor_m2.h+=2;
                myhsvColor_s1.h+=2;
                myhsvColor_s2.h+=2;  
                jumpTimer = 0;
            }
            jumpTimer++;
            break;
        case 3:   //同步调色
            if(jumpTimer >= speed*10){
                myhsvColor_h1.h+=19;
                myhsvColor_h2.h+=19;
                myhsvColor_m1.h+=19;
                myhsvColor_m2.h+=19;
                myhsvColor_s1.h+=19;
                myhsvColor_s2.h+=19;
                jumpTimer = 0;
            }
            jumpTimer++;
            break;
        case 4:  //渐变跳色
            if(jumpTimer >= speed*10){
                myhsvColor_h1.h+=19;
                myhsvColor_h2.h+=19;
                myhsvColor_m1.h+=19;
                myhsvColor_m2.h+=19;
                myhsvColor_s1.h+=19;
                myhsvColor_s2.h+=19;
                jumpTimer = 0;
            }
            jumpTimer++;
            break;
        case 5:  //呼吸模式 
            if(jumpTimer >= speed/2){
                // if(myhsvColor_s2.v == 0 || myhsvColor_s2.v == 1) breath_flag = 2;
                if(myhsvColor_s2.v <= 80) breath_flag = 2;
                else if(myhsvColor_s2.v == 255) breath_flag = -2;
                myhsvColor_h1.v+=breath_flag;
                myhsvColor_h2.v+=breath_flag;
                myhsvColor_m1.v+=breath_flag;
                myhsvColor_m2.v+=breath_flag;
                myhsvColor_s1.v+=breath_flag;
                myhsvColor_s2.v+=breath_flag; 
                jumpTimer = 0;
            }
            jumpTimer++;
            break;
    }
}
void set_turnPage(){
    //翻页动画开始
    if(turnPage_flag != 4){
      if(c_hours_1   == 0 && c_hours_1_flag   == 1){ leds[0] = CRGB::Black;  leds[1] = CRGB::Black;}  //先清除零位
      if(c_hours_2   == 0 && c_hours_2_flag   == 1){ leds[20] = CRGB::Black;  leds[21] = CRGB::Black;}//先清除零位
      if(c_minutes_1 == 0 && c_minutes_1_flag == 1){ leds[40] = CRGB::Black;  leds[41] = CRGB::Black;}//先清除零位
      if(c_minutes_2 == 0 && c_minutes_2_flag == 1){ leds[60] = CRGB::Black;  leds[61] = CRGB::Black;}//先清除零位
      if(c_seconds_1 == 0 && c_seconds_1_flag == 1){ leds[80] = CRGB::Black;  leds[81] = CRGB::Black;}//先清除零位
      if(c_seconds_2 == 0 && c_seconds_2_flag == 1){ leds[100] = CRGB::Black;  leds[101] = CRGB::Black;}//先清除零位
      if((c_minutes_1 == 0 && c_minutes_1_flag ==1)||(c_seconds_1 == 0 && c_seconds_1_flag == 1)||(c_minutes_2 == 0 && c_minutes_2_flag == 1)|| (c_seconds_2 == 0 && c_seconds_2_flag == 1)||(c_hours_1 == 0 && c_hours_1_flag == 1)||(c_hours_2 == 0 && c_hours_2_flag == 1)){
        for(int i = 9; i >= 0; i--){
          if(c_hours_1   == 0 && c_hours_1_flag   == 1){ leds[2*i +  0] = myhsvColor_h1;  leds[2*i +  1] = myhsvColor_h1;}
          if(c_hours_2   == 0 && c_hours_2_flag   == 1){ leds[2*i + 20] = myhsvColor_h2;  leds[2*i + 21] = myhsvColor_h2;}
          if(c_minutes_1 == 0 && c_minutes_1_flag == 1){ leds[2*i + 40] = myhsvColor_m1;  leds[2*i + 41] = myhsvColor_m1;}
          if(c_minutes_2 == 0 && c_minutes_2_flag == 1){ leds[2*i + 60] = myhsvColor_m2;  leds[2*i + 61] = myhsvColor_m2;}
          if(c_seconds_1 == 0 && c_seconds_1_flag == 1){ leds[2*i + 80] = myhsvColor_s1;  leds[2*i + 81] = myhsvColor_s1;}
          if(c_seconds_2 == 0 && c_seconds_2_flag == 1){ leds[2*i +100] = myhsvColor_s2;  leds[2*i +101] = myhsvColor_s2;}
          if(turnPage_flag == 2|| turnPage_flag == 3){
            myhsvColor_h1.h+=5;
            myhsvColor_h2.h+=5;
            myhsvColor_m1.h+=5;
            myhsvColor_m2.h+=5;
            myhsvColor_s1.h+=5;
            myhsvColor_s2.h+=5;
          }else {}
          delay(30);
          if(turnPage_flag == 0|| turnPage_flag == 2){
            if(c_hours_1   == 0 && c_hours_1_flag   == 1){ leds[2*i +  0] = CRGB::Black;  leds[2*i +  1] = CRGB::Black;}
            if(c_hours_2   == 0 && c_hours_2_flag   == 1){ leds[2*i + 20] = CRGB::Black;  leds[2*i + 21] = CRGB::Black;}
            if(c_minutes_1 == 0 && c_minutes_1_flag == 1){ leds[2*i + 40] = CRGB::Black;  leds[2*i + 41] = CRGB::Black;}
            if(c_minutes_2 == 0 && c_minutes_2_flag == 1){ leds[2*i + 60] = CRGB::Black;  leds[2*i + 61] = CRGB::Black;}
            if(c_seconds_1 == 0 && c_seconds_1_flag == 1){ leds[2*i + 80] = CRGB::Black;  leds[2*i + 81] = CRGB::Black;}
            if(c_seconds_2 == 0 && c_seconds_2_flag == 1){ leds[2*i +100] = CRGB::Black;  leds[2*i +101] = CRGB::Black;}
          }else{}
        }
        if(c_hours_1   == 0) c_hours_1_flag = 0;
        if(c_hours_2   == 0) c_hours_2_flag = 0;
        if(c_minutes_1 == 0) c_minutes_1_flag = 0;
        if(c_minutes_2 == 0) c_minutes_2_flag = 0;
        if(c_seconds_1 == 0) c_seconds_1_flag = 0;
        if(c_seconds_2 == 0) c_seconds_2_flag = 0;
      }
    }else {}//turnPage_flag等于4的时候 就没有翻页动画
    //翻页动画结束
}
void send_system_time(){
    StaticJsonDocument<500> doc;
    doc["timing_on"] = eeprom_save.timing_on;
    doc["open_time"] = eeprom_save.open_time;
    doc["close_time"] = eeprom_save.close_time;
    doc["sys_time"] = String(millis());
    String bufferString;
    serializeJson(doc, bufferString);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(bufferString.c_str());
    Udp.endPacket();
    Serial.println(String(millis()).c_str());
}
void check_UDP_recived_parse(){
    if(Udp.parsePacket()){
        String bufferString = Udp.readString();
        Serial.println(bufferString);
        StaticJsonDocument<500> doc;
        deserializeJson(doc, bufferString);
        JsonObject root = doc.as<JsonObject>();
        if(root.containsKey("display_flag")){
          display_flag = root["display_flag"];
        }
        else if(root.containsKey("mode_flag")){
          mode_flag = root["mode_flag"];
        }
        else if(root.containsKey("turnPage_flag")){
          turnPage_flag = root["turnPage_flag"];
        }
        else if(root.containsKey("speed")){
          speed = root["speed"];
        }
        else if(root.containsKey("isOn")){
          isOn = root["isOn"];
        }
        else if(root.containsKey("get")){
          if(root["get"]=="time"){
            send_system_time();
          }
        }
        else if(root.containsKey("boot_mode")){
          if(root["boot_mode"] == true ){
            eeprom_save.boot_mode = 1;
            EEPROM.begin(1024);
            EEPROM.put(0, eeprom_save);
            EEPROM.end();
            delay(100);
            ESP.restart();
          }
        }
        else if(root.containsKey("color")){
            switch_value[0] = root["switch_value"][0];
            switch_value[1] = root["switch_value"][1];
            switch_value[2] = root["switch_value"][2];
            switch_value[3] = root["switch_value"][3];
            switch_value[4] = root["switch_value"][4];
            switch_value[5] = root["switch_value"][5];

            color[0] = root["color"][0];
            color[1] = root["color"][1];
            color[2] = root["color"][2];

            if(switch_value[0] == 1) {myhsvColor_h1 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
            if(switch_value[1] == 1) {myhsvColor_h2 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
            if(switch_value[2] == 1) {myhsvColor_m1 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
            if(switch_value[3] == 1) {myhsvColor_m2 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
            if(switch_value[4] == 1) {myhsvColor_s1 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
            if(switch_value[5] == 1) {myhsvColor_s2 = rgb2hsv_approximate(CRGB(color[0], color[1], color[2]));}  
        }
    }
}
void  check_every_num_ainamation(){
    for (int i = 0; i < 10; i++){
        myhsvColor.h += 2;
        leds[2*i      ] =myhsvColor;leds[2*i + 1 ] =myhsvColor;
        leds[2*i + 20 ] =myhsvColor;leds[2*i + 21] =myhsvColor;
        leds[2*i + 40 ] =myhsvColor;leds[2*i + 41] =myhsvColor;
        leds[2*i + 60 ] =myhsvColor;leds[2*i + 61] =myhsvColor;
        leds[2*i + 80 ] =myhsvColor;leds[2*i + 81] =myhsvColor;
        leds[2*i + 100] =myhsvColor;leds[2*i +101] =myhsvColor;
        delay(50);
        FastLED.clear();
    }
    for (int i = 9; i >= 0; i--){
        myhsvColor.h += 2;
        leds[2*i     ] =myhsvColor;leds[2*i + 1 ] =myhsvColor;
        leds[2*i + 20] =myhsvColor;leds[2*i + 21] =myhsvColor;
        leds[2*i + 40] =myhsvColor;leds[2*i + 41] =myhsvColor;
        leds[2*i + 60] =myhsvColor;leds[2*i + 61] =myhsvColor;
        leds[2*i + 80] =myhsvColor;leds[2*i + 81] =myhsvColor;
        leds[2*i +100] =myhsvColor;leds[2*i +101] =myhsvColor;
        delay(45);
        FastLED.clear();
    }
}
void timer_ticker_callback(){
    c_seconds++; if(c_seconds > 59) {c_seconds = 0; c_minutes++;}
    if(c_minutes > 59) {c_minutes = 0;c_hours++;};
    if(c_hours > 23) c_hours = 0;
    // if(c_hours > (E_h12?12:23)) c_hours = (E_h12?1:0);

    uint8_t c_hours_temp;
    if (eeprom_save.E_h12)
    {
        if (c_hours >= 0 && c_hours <= 12) c_hours_temp = c_hours;
        else if (c_hours >=13 && c_hours <= 23 ) c_hours_temp = c_hours - 12;
        c_hours_1   = c_hours_temp/10;
        c_hours_2   = c_hours_temp%10;   
    }
    else 
    {
        c_hours_1   = c_hours/10;
        c_hours_2   = c_hours%10;
    }
    
    c_minutes_1 = c_minutes/10;
    c_minutes_2 = c_minutes%10;
    c_seconds_1 = c_seconds/10;
    c_seconds_2 = c_seconds%10;

    EVERY_N_SECONDS(9) {c_seconds_2_flag = 1; timing_on_task();}//每9秒获取一次温度
    EVERY_N_SECONDS(59){c_seconds_1_flag = 1;}
    EVERY_N_MINUTES(9) {c_minutes_2_flag = 1;}
    EVERY_N_MINUTES(59){c_minutes_1_flag = 1;}
    EVERY_N_HOURS  (9) {c_hours_1_flag   = 1;}//每9小时获取一次日期
    EVERY_N_HOURS  (19){c_hours_2_flag   = 1; timing_Task_once1 = timing_Task_once2 = true;}
}
void timing_on_task(){
    if (eeprom_save.timing_on)
    {
        char* temp; 
        if (c_hours == (uint8_t)strtol(eeprom_save.open_time, &temp, 10) && c_minutes == (uint8_t)strtol(temp+1, NULL, 10) && timing_Task_once1) {isOn = true; timing_Task_once1 = false;}
        if (c_hours == (uint8_t)strtol(eeprom_save.close_time, &temp, 10) && c_minutes == (uint8_t)strtol(temp+1, NULL, 10) && timing_Task_once2) {isOn = false; timing_Task_once2 = false;}
    } 
}

ICACHE_RAM_ATTR void buttonInterrupt(){
	if (digitalRead(D7) == LOW){
		keydown = true;
		keyup = false;
	}
	else {
		keydown = false;
		keyup = true;
	}	
};
void FastLED_show_ticker_callback(){
    FastLED.show();
    MDNS.update();
}
void handleRootRequest(){
    Serial.println("用户发来了请求");
    Serial.println(server.method() == HTTP_GET ? "GET" : "POST");
    if (server.method() == HTTP_POST)
    {
        if (server.hasArg("ssid") || server.hasArg("pswd") || server.hasArg("E_h12") )
        {

            if (server.arg("ssid").c_str()[0] != '\0')
            {
                strcpy(eeprom_save.ssid, server.arg("ssid").c_str());
                strcpy(eeprom_save.pswd, server.arg("pswd").c_str());
            }
            if (server.arg("E_h12") == "true") eeprom_save.E_h12 = true;
            else eeprom_save.E_h12 = false;

            if (server.arg("timing_on") == "true") eeprom_save.timing_on = true;
            else eeprom_save.timing_on = false;

            strcpy(eeprom_save.open_time, server.arg("open_time").c_str());
            strcpy(eeprom_save.close_time, server.arg("close_time").c_str());

            StaticJsonDocument<200> doc;
            deserializeJson(doc, server.arg("set_time"));
            JsonObject root = doc.as<JsonObject>();

            if (server.arg("set_time_flag") == "true")
            {
                Serial.println("校准时间");
                Clock.setYear((uint8_t)root["year"]);
                Clock.setMonth((uint8_t)root["month"]);
                Clock.setDate((uint8_t)root["day"]);
                Clock.setHour((uint8_t)root["hour"]);
                Clock.setMinute((uint8_t)root["minute"]);
                Clock.setSecond((uint8_t)root["second"]);
            }

            eeprom_save.boot_mode = false; //设置正常启动模式
            EEPROM.begin(1024);
            EEPROM.put(0, eeprom_save);
            EEPROM.end();
            server.send(200, "text/plain", "配置信息已保存!");
            delay(100);
            ESP.reset();
        }
    } 
    else
    {
        server.send(200, "text/html", webpage_html);
    }
    
}
