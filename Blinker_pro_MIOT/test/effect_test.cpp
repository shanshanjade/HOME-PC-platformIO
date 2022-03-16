#define BLINKER_PRO_ESP
#define BLINKER_OTA_VERSION_CODE "0.1.1"

#include <FastLED.h>
#include <DS3231.h>
#include <Wire.h>
#include <Blinker.h>

#define NUM_LEDS 120
#define DATA_PIN D3
#define LED_TYPE WS2812
#define COLOR_ORDER GRB
//密钥
char type[] = "GylscuaRWRec";
char auth[] = "aLJbbeqFxU1B";
//时钟相关
DS3231 Clock;
bool h12;
bool PM;
bool Century;
//WS2812相关
CRGB leds[NUM_LEDS];
CRGB myrgbColor(255, 0, 0);
CHSV myhsvColor(0,255,255);

CHSV myhsvColor_h1(0,255,255);
CHSV myhsvColor_h2(0,255,255);
CHSV myhsvColor_m1(0,255,255);
CHSV myhsvColor_m2(0,255,255);
CHSV myhsvColor_s1(0,255,255);
CHSV myhsvColor_s2(0,255,255);
//时间变量
uint8_t c_hours = 0;
uint8_t c_minutes = 0;
uint8_t c_seconds = 0;
uint8_t c_years = 0;
uint8_t c_months = 0;
uint8_t c_days = 0;
int8_t  c_temperatures = 0;

uint8_t c_hours_1;
uint8_t c_hours_2;
uint8_t c_minutes_1;
uint8_t c_minutes_2;
uint8_t c_seconds_1;
uint8_t c_seconds_2;
//翻页动画相关
bool c_hours_1_flag = 1;
bool c_hours_2_flag = 1;
bool c_minutes_1_flag = 1;
bool c_minutes_2_flag = 1;
bool c_seconds_1_flag = 1;
bool c_seconds_2_flag = 1;
//按键相关
int press_counter = 0;
bool key_flag = false;
//模式相关
uint8_t mode_flag = 2;
uint8_t display_flag = 0;
uint8_t turnPage_flag = 3;
uint8_t jumpTimer = 0;
uint8_t jumpTimer_temp = 0;
int8_t temperature_i = 0;
int8_t breath_flag = 1;
//开关
bool isOn = true;
// bool isBlinker = false;

//BLinker控件
BlinkerRGB col_abc("col-abc");
BlinkerButton btn_h1("btn-h1");
BlinkerButton btn_h2("btn-h2");
BlinkerButton btn_m1("btn-m1");
BlinkerButton btn_m2("btn-m2");
BlinkerButton btn_s1("btn-s1");
BlinkerButton btn_s2("btn-s2");
BlinkerButton btn_rev("btn-rev");
BlinkerButton btn_page("btn-page");
BlinkerButton btn_mode("btn-mode");
BlinkerButton btn_date("btn-date");
BlinkerButton btn_temp("btn-temp");
BlinkerButton btn_isOn("btn-isOn");
BlinkerText tex_abc("tex-abc");

bool btn_h1_flag = 0;
bool btn_h2_flag = 0;
bool btn_m1_flag = 0;
bool btn_m2_flag = 0;
bool btn_s1_flag = 0;
bool btn_s2_flag = 0;

void col_abc_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value){
  myrgbColor.r = r_value; myrgbColor.g = g_value; myrgbColor.b = b_value;
  myhsvColor = rgb2hsv_approximate(myrgbColor);
  myhsvColor.v = bright_value;
  if(btn_h1_flag) myhsvColor_h1 = myhsvColor;
  if(btn_h2_flag) myhsvColor_h2 = myhsvColor;
  if(btn_m1_flag) myhsvColor_m1 = myhsvColor;
  if(btn_m2_flag) myhsvColor_m2 = myhsvColor;
  if(btn_s1_flag) myhsvColor_s1 = myhsvColor;
  if(btn_s2_flag) myhsvColor_s2 = myhsvColor;
}
void btn_h1_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_h1_flag = 1; btn_h1.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_h1_flag = 0; btn_h1.print("off");
  }
}
void btn_h2_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_h2_flag = 1; btn_h2.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_h2_flag = 0; btn_h2.print("off");
  }
}
void btn_m1_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_m1_flag = 1; btn_m1.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_m1_flag = 0; btn_m1.print("off");
  }
}
void btn_m2_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_m2_flag = 1; btn_m2.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_m2_flag = 0; btn_m2.print("off");
  }
}
void btn_s1_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_s1_flag = 1; btn_s1.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_s1_flag = 0; btn_s1.print("off");
  }
}
void btn_s2_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    btn_s2_flag = 1; btn_s2.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_s2_flag = 0; btn_s2.print("off");
  }
}

void btn_rev_callback(const String & state){
  btn_h1_flag = !btn_h1_flag; if(btn_h1_flag) btn_h1.print("on"); else btn_h1.print("off"); 
  btn_h2_flag = !btn_h2_flag; if(btn_h2_flag) btn_h2.print("on"); else btn_h2.print("off"); 
  btn_m1_flag = !btn_m1_flag; if(btn_m1_flag) btn_m1.print("on"); else btn_m1.print("off"); 
  btn_m2_flag = !btn_m2_flag; if(btn_m2_flag) btn_m2.print("on"); else btn_m2.print("off"); 
  btn_s1_flag = !btn_s1_flag; if(btn_s1_flag) btn_s1.print("on"); else btn_s1.print("off"); 
  btn_s2_flag = !btn_s2_flag; if(btn_s2_flag) btn_s2.print("on"); else btn_s2.print("off"); 
  if(state == BLINKER_CMD_ON){
    btn_rev.print("on");
  }else if(state == BLINKER_CMD_OFF){
    btn_rev.print("on");
  }
}
void btn_page_callback(const String & state){
  key_flag = true; press_counter = 20;
}
void btn_mode_callback(const String & state){
  key_flag = true; press_counter = 8;
}
void btn_date_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    key_flag = true; press_counter = 40; display_flag = 0;
    btn_date.print("on"); btn_temp.print("off");
  }else if(state == BLINKER_CMD_OFF){
    key_flag = true; press_counter = 40; display_flag = 2;
    btn_date.print("off");
  }
}
void btn_temp_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    key_flag = true; press_counter = 40; display_flag = 1;
    btn_temp.print("on"); btn_date.print("off");
  }else if(state == BLINKER_CMD_OFF){
    key_flag = true; press_counter = 40; display_flag = 2;
    btn_temp.print("off");
  }
}
void btn_isOn_callback(const String & state){
  isOn = !isOn;
  // if(state == BLINKER_CMD_ON){
  //  isOn = true; 
  //  btn_isOn.print("on");
  // }else if(state == BLINKER_CMD_OFF){
  //  isOn = false; 
  //  btn_isOn.print("off");
  // }
}

void heartbeat(){
  c_temperatures = Clock.getTemperature();
  if(Blinker.status() == PRO_DEV_CONNECTED){
    tex_abc.icon("fas fa-signal-alt");
    tex_abc.print("已连接");
  }else{
    tex_abc.icon("fas fa-signal-alt-slash");
    tex_abc.print("未连接");
  }
}


void setup(){
  Serial.begin(115200);
  Wire.begin();
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();
  Blinker.begin(auth,type);
  c_hours  = Clock.getHour(h12,PM);
  c_minutes= Clock.getMinute();
  c_seconds= Clock.getSecond();
  c_years  = Clock.getYear();
  c_months = Clock.getMonth(Century);
  c_days   = Clock.getDate();
  c_temperatures = Clock.getTemperature();
  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(D7, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(85);

  col_abc.attach(col_abc_callback);
  btn_h1.attach(btn_h1_callback);
  btn_h2.attach(btn_h2_callback);
  btn_m1.attach(btn_m1_callback);
  btn_m2.attach(btn_m2_callback);
  btn_s1.attach(btn_s1_callback);
  btn_s2.attach(btn_s2_callback);
  btn_rev.attach(btn_rev_callback);
  btn_page.attach(btn_page_callback);
  btn_mode.attach(btn_mode_callback);
  btn_date.attach(btn_date_callback);
  btn_temp.attach(btn_temp_callback);
  btn_isOn.attach(btn_isOn_callback);
  Blinker.attachHeartbeat(heartbeat); 
  Blinker.deleteTimer();
  for (int i = 0; i < 10; i++){
    leds[2*i      ] =myhsvColor;leds[2*i + 1 ] =myhsvColor;
    leds[2*i + 20 ] =myhsvColor;leds[2*i + 21] =myhsvColor;
    leds[2*i + 40 ] =myhsvColor;leds[2*i + 41] =myhsvColor;
    leds[2*i + 60 ] =myhsvColor;leds[2*i + 61] =myhsvColor;
    leds[2*i + 80 ] =myhsvColor;leds[2*i + 81] =myhsvColor;
    leds[2*i + 100] =myhsvColor;leds[2*i +101] =myhsvColor;
    FastLED.show();
    delay(50);
    FastLED.clear();
  }
  for (int i = 9; i >= 0; i--){
    leds[2*i     ] =myhsvColor;leds[2*i + 1 ] =myhsvColor;
    leds[2*i + 20] =myhsvColor;leds[2*i + 21] =myhsvColor;
    leds[2*i + 40] =myhsvColor;leds[2*i + 41] =myhsvColor;
    leds[2*i + 60] =myhsvColor;leds[2*i + 61] =myhsvColor;
    leds[2*i + 80] =myhsvColor;leds[2*i + 81] =myhsvColor;
    leds[2*i +100] =myhsvColor;leds[2*i +101] =myhsvColor;
    FastLED.show();
    delay(50);
    FastLED.clear();
  }
}


void loop(){
  unsigned long startTime = millis();
  FastLED.clear();

  c_hours_1   = c_hours/10;
  c_hours_2   = c_hours%10;
  c_minutes_1 = c_minutes/10;
  c_minutes_2 = c_minutes%10;
  c_seconds_1 = c_seconds/10;
  c_seconds_2 = c_seconds%10;

  if(isOn){
    if(display_flag == 0){//如果是时间
      leds[2*(c_hours_1)       ] = myhsvColor_h1; leds[2*(c_hours_1)   + 1 ] = myhsvColor_h1;
      leds[2*(c_hours_2)   + 20] = myhsvColor_h2; leds[2*(c_hours_2)   + 21] = myhsvColor_h2;
      leds[2*(c_minutes_1) + 40] = myhsvColor_m1; leds[2*(c_minutes_1) + 41] = myhsvColor_m1;
      leds[2*(c_minutes_2) + 60] = myhsvColor_m2; leds[2*(c_minutes_2) + 61] = myhsvColor_m2;
      leds[2*(c_seconds_1) + 80] = myhsvColor_s1; leds[2*(c_seconds_1) + 81] = myhsvColor_s1;
      leds[2*(c_seconds_2) +100] = myhsvColor_s2; leds[2*(c_seconds_2) +101] = myhsvColor_s2;
    }else if(display_flag == 1){ //如果是日期
      turnPage_flag = 4;//关闭翻页动画
      leds[2*(c_years/10)       ] = myhsvColor_h1; leds[2*(c_years/10)   + 1 ] = myhsvColor_h1;
      leds[2*(c_years%10)   + 20] = myhsvColor_h2; leds[2*(c_years%10)   + 21] = myhsvColor_h2;
      leds[2*(c_months/10) + 40] = myhsvColor_m1; leds[2*(c_months/10) + 41] = myhsvColor_m1;
      leds[2*(c_months%10) + 60] = myhsvColor_m2; leds[2*(c_months%10) + 61] = myhsvColor_m2;
      leds[2*(c_days/10) + 80] = myhsvColor_s1; leds[2*(c_days/10) + 81] = myhsvColor_s1;
      leds[2*(c_days%10) +100] = myhsvColor_s2; leds[2*(c_days%10) +101] = myhsvColor_s2;
    }else if(display_flag == 2){//如果是温度
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
    //模式选择
    if(mode_flag == 0){ //全流光，需要给初始颜色(6个全部一样)，点击无效/关灯
      myhsvColor_h1.h++;
      myhsvColor_h2.h++;
      myhsvColor_m1.h++;
      myhsvColor_m2.h++;
      myhsvColor_s1.h++;
      myhsvColor_s2.h++;
    }else if(mode_flag == 1){ //单色，需要给初始颜色(6个全部一样)，点击变色

    }else if(mode_flag == 2){ //横流光，需要给初始颜色(6个间隔色差17)，点击关灯
      myhsvColor_h1.h++;
      myhsvColor_h2.h++;
      myhsvColor_m1.h++;
      myhsvColor_m2.h++;
      myhsvColor_s1.h++;
      myhsvColor_s2.h++;
    }else if(mode_flag == 3){ //全跳色，需要给初始颜色(6个全部一样)，点击关灯
      if(jumpTimer == 30){
        myhsvColor_h1.h+=17;
        myhsvColor_h2.h+=17;
        myhsvColor_m1.h+=17;
        myhsvColor_m2.h+=17;
        myhsvColor_s1.h+=17;
        myhsvColor_s2.h+=17;
        jumpTimer = 0;
      }
      jumpTimer++;
    }else if(mode_flag == 4){ //横跳色，需要给初始颜色(6个间隔色差17)，点击关灯
      if(jumpTimer == 30){
        myhsvColor_h1.h+=17;
        myhsvColor_h2.h+=17;
        myhsvColor_m1.h+=17;
        myhsvColor_m2.h+=17;
        myhsvColor_s1.h+=17;
        myhsvColor_s2.h+=17;
        jumpTimer = 0;
      }
      jumpTimer++;
    }else if(mode_flag == 5){ //呼吸，需要给初始颜色(6个全部一样)，点击变色
      if(myhsvColor_s2.v == 0 || myhsvColor_s2.v == 1) breath_flag = 2;
      else if(myhsvColor_s2.v == 255) breath_flag = -2;
      myhsvColor_h1.v+=breath_flag;
      myhsvColor_h2.v+=breath_flag;
      myhsvColor_m1.v+=breath_flag;
      myhsvColor_m2.v+=breath_flag;
      myhsvColor_s1.v+=breath_flag;
      myhsvColor_s2.v+=breath_flag;
    }
    //模式选择
    
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
          FastLED.show();

          if(turnPage_flag == 2|| turnPage_flag == 3){
            myhsvColor_h1.h+=5;
            myhsvColor_h2.h+=5;
            myhsvColor_m1.h+=5;
            myhsvColor_m2.h+=5;
            myhsvColor_s1.h+=5;
            myhsvColor_s2.h+=5;
          }else {}

          FastLED.delay(45);

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
    FastLED.show();
  }else{
    FastLED.showColor(CRGB::Black);
  }
  
  
  EVERY_N_MILLISECONDS(100){  //按键计时器，每隔100毫秒自增1
    press_counter++;
  }
  EVERY_N_SECONDS(1){
    c_seconds++; if(c_seconds > 59) {c_seconds = 0; c_minutes++;}
    if(c_minutes > 59) {c_minutes = 0;c_hours++;};
    if(c_hours > 23) c_hours = 0;
  }
  EVERY_N_SECONDS(9) {c_seconds_2_flag = 1;  if(display_flag == 2)c_temperatures = Clock.getTemperature();}//每9秒获取一次温度
  EVERY_N_SECONDS(59){c_seconds_1_flag = 1;}
  EVERY_N_MINUTES(9) {c_minutes_2_flag = 1;}
  EVERY_N_MINUTES(59){c_minutes_1_flag = 1;}
  EVERY_N_HOURS  (9) {c_hours_1_flag   = 1;  if(display_flag == 1)c_days = Clock.getDate();}//每9小时获取一次日期
  EVERY_N_HOURS  (19){c_hours_2_flag   = 1;}

  if (digitalRead(D7) == LOW) key_flag = true; //按键按下
  else if (key_flag == true) //按键被放开，但按键事件还未被处理
  {
    if(press_counter <=    1){}//消除抖动
    else if(press_counter <=   4){ //小于0.4s 点击
      if(mode_flag == 0 || mode_flag == 2 || mode_flag == 3 || mode_flag == 4 ) isOn = !isOn;
      else {
        myhsvColor_h1.h+=17;
        myhsvColor_h2.h+=17;
        myhsvColor_m1.h+=17;
        myhsvColor_m2.h+=17;
        myhsvColor_s1.h+=17;
        myhsvColor_s2.h+=17;
      }

    } 
    else if (press_counter <=  10){ // 0.4-1s 改变模式
      mode_flag++; if(mode_flag > 5)mode_flag = 0;
      if(mode_flag == 0) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(0,255,255);
      else if(mode_flag == 1) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(0,255,255);
      else if(mode_flag == 2) {myhsvColor_h1.h+=0;myhsvColor_h2.h+=17;myhsvColor_m1.h+=34;myhsvColor_m2.h+=51;myhsvColor_s1.h+=68;myhsvColor_s2.h+=85;}
      else if(mode_flag == 3) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(0,255,255);
      else if(mode_flag == 4) {myhsvColor_h1.h+=0;myhsvColor_h2.h+=17;myhsvColor_m1.h+=34;myhsvColor_m2.h+=51;myhsvColor_s1.h+=68;myhsvColor_s2.h+=85;}
      else if(mode_flag == 5) myhsvColor_h1=myhsvColor_h2=myhsvColor_m1=myhsvColor_m2=myhsvColor_s1=myhsvColor_s2=CHSV(0,255,255);
      // FastLED.showColor(CRGB(0,255,255));
    }
    else if (press_counter <=  25){ //1-2.5s  翻页动画
      turnPage_flag++; if(turnPage_flag > 4)turnPage_flag = 0; 
      // FastLED.showColor(CRGB::Yellow);
    }
    else if (press_counter <=  50){ //  2.5s-5s  改变显示内容时间/日期/温度
      display_flag++; if(display_flag > 2)display_flag = 0;      
      // FastLED.showColor(CRGB::Red);
    }
    else if (press_counter <= 100){   // 5s-10s 启动blinker
      // isBlinker = true;
      // Blinker.begin(auth,type);
      // Serial.println("启动blinker");
      // FastLED.showColor(CRGB::Green);
    }
    else if (press_counter <= 200){  //10s-20s 重置配网信息
      // Blinker.reset();
      // Serial.println("重置配网信息");
      // FastLED.showColor(CRGB::Blue);
    }
    key_flag = false; 
    delay(200);
  } //按键事件处理完毕
  else press_counter = 0;  //计时器清零
  // Serial.println(millis()-startTime);//检测一个循环占用多少毫秒 目前是25-28ms
  if((millis()-startTime) < 30){delay(30 - int(millis() - startTime));}
  Blinker.run();
}