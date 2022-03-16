#define BLINKER_PRO_ESP
#define BLINKER_MIOT_LIGHT
#define BLINKER_NO_BUTTON
#define BLINKER_OTA_VERSION_CODE "0.1.1"
#include <FastLED.h>
#include <Blinker.h>
#define NUM_LEDS 16             // LED灯珠数量
#define DATA_PIN D3              // Arduino输出控制信号引脚
#define LED_TYPE WS2812         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
uint8_t myBright = 255;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
CRGB leds[NUM_LEDS];            // 建立光带leds对象
CRGB myRGBcolor(0,  0,  0);     //建立我的RGB颜色
CRGB myRGBcolorA(0, 0,  0);
CHSV myHSVcolor(0,  255,  255); //建立我的HSV颜色
CRGBPalette16 myColorPalette[] = {RainbowColors_p, RainbowStripeColors_p, PartyColors_p, LavaColors_p, ForestColors_p, OceanColors_p, HeatColors_p,CloudColors_p};
uint8_t colorPaletteIndex = 0;    //仿色函数使用变量 色板序号    做8个效果
uint8_t beginColorIndex = 0;       //仿色函数使用变量 开始颜色  做动画
uint8_t colorPaletteDelay = 0;    //仿色函数使用变量 延迟时间  用滑条做效果
uint8_t effectIndex = 0;          //特效函数使用变量 效果函数索引
uint8_t changeIndex = 0;          //变色函数使用变量 效果函数索引 用色盘做开始颜色
unsigned long currentTime = 0;
unsigned long setTime = 0;
char type[] = "WBYBd4vKVJbZ";
char auth[] = "FDrFirtY361y";

void Effect0();
void Effect1();
void Effect2();
void Effect3();
void Effect4();
void Effect5();
void Effect6();
void Effect7();

void SimulateColor();

void Change0(); //柔和渐变
void Change1(); //彩虹跳变
void Change2(); //反色渐变
void Change3(); //反色闪烁
void Change4(); //对比色渐变
void Change5(); //对比色闪烁

void Breath();



BlinkerButton button1("btn-abc");
BlinkerRGB rgb1("col-abc");
BlinkerTab tab1("tab-abc");
BlinkerSlider slider1("ran-abc");
bool button1state;
uint8_t tab1state = 1;

void noButtonReset()
{
    Blinker.reset();
}

void button1_callback(const String & state){
  if(state == BLINKER_CMD_ON){
    button1.color("#FF0000");
    button1.print("on");
    myHSVcolor.h = random8();//以上语句将返回0-255之间的随机数值。
    fill_solid(leds,  NUM_LEDS, myHSVcolor);
    FastLED.setBrightness(255);
    hsv2rgb_rainbow(myHSVcolor, myRGBcolor);
    rgb1.brightness(255);
    rgb1.print(myRGBcolor.r,  myRGBcolor.g, myRGBcolor.b, 255);
    button1state = 1;
  }else if(state == BLINKER_CMD_OFF){
    button1.color("#000000");
    button1.print("off");
    FastLED.setBrightness(0); 
    rgb1.brightness(0);
    rgb1.print(255,  255, 255, 0);
    button1state = 0;
  }
  FastLED.show();
}

void rgb1_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value){
  if(button1state == 1){   
    if(myRGBcolor.r != r_value || myRGBcolor.g != g_value || myRGBcolor.b != b_value ) myRGBcolorA = myRGBcolor;
    myRGBcolor.r = r_value;
    myRGBcolor.g = g_value;
    myRGBcolor.b = b_value;
    myBright = bright_value;
    myHSVcolor = rgb2hsv_approximate(myRGBcolor);
      switch (tab1state){
          case 0:       //呼吸模式下只变颜色
            fill_solid(leds,  NUM_LEDS, myRGBcolor); 
            break;
          case 1:       //单色模式下变颜色和亮度
            fill_solid(leds,  NUM_LEDS, myRGBcolor);
            FastLED.setBrightness(bright_value);
            break;
          case 2:       //变色模式下改变颜色和亮度
            myHSVcolor.s = 255;
            myHSVcolor.v = bright_value;
            break;
          case 3:       //仿色模式下 用亮度改变延迟时间
            break;
          case 4:       //特效模式下什么也不做
            break;
          default:
            break;
      }
    FastLED.show();
    // if(bright_value == 0){
    //   button1state = 0;
    //   button1.color("#000000");
    //   button1.print("off");
    // }else {
    //   button1state = 1;
    //   button1.color("#FF0000");
    //   button1.print("on");
    // }
  }
  rgb1.print(r_value,g_value,b_value,bright_value);
}

void tab1_callback(uint8_t tab_set){
  if(button1state == 1){
    switch (tab_set)
         {
          case BLINKER_CMD_TAB_0 :
              tab1state = 0;
              break;
          case BLINKER_CMD_TAB_1 :
              tab1state = 1;
              break;
          case BLINKER_CMD_TAB_2 :
              tab1state = 2;
              changeIndex++;
              if(changeIndex > 5){changeIndex = 0;}
              break;
          case BLINKER_CMD_TAB_3 :
              tab1state = 3;
              colorPaletteIndex = random8(8);
              break;
          case BLINKER_CMD_TAB_4 :
              tab1state = 4;
              effectIndex++;
              if(effectIndex > 7){effectIndex = 0;}
              break;
          default:
              break;
          }
    }
}
void tab1_feedback(){}

void slider1_callback(int32_t value){
  currentTime = millis();
  setTime = value * 60 * 1000;
}

void miotPowerState(const String & state)
{
    if (state == BLINKER_CMD_ON) {
      button1state = 1;
      FastLED.setBrightness(255);
      FastLED.show();
      button1.print("on");
      BlinkerMIOT.powerState("on");
      BlinkerMIOT.print();
    }
    else if (state == BLINKER_CMD_OFF) {
      button1state = 0;
      FastLED.setBrightness(0);
      FastLED.show();
      button1.print("off");
      BlinkerMIOT.powerState("off");
      BlinkerMIOT.print();
    }
}
void miotBright(const String & bright)
{
    myBright = bright.toInt();
    FastLED.setBrightness(myBright);
    FastLED.show();
    rgb1.brightness(myBright);
    rgb1.print(myRGBcolor.r,  myRGBcolor.g, myRGBcolor.b, myBright);
    BlinkerMIOT.brightness(myBright);
    BlinkerMIOT.print();
}


int32_t tempColor;
void miotColor(int32_t color)
{
    tempColor = color;
    BLINKER_LOG("need set color: ", color);
    myRGBcolor.r = color >> 16 & 0xFF;
    myRGBcolor.g = color >>  8 & 0xFF;
    myRGBcolor.b = color       & 0xFF;
    FastLED.showColor(myRGBcolor);
    BlinkerMIOT.color(color);
    BlinkerMIOT.print();
}

void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(button1state ? "on" : "off");
            BlinkerMIOT.color(tempColor);
            BlinkerMIOT.brightness(myBright);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(button1state ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_COLOR_NUMBER :
            BLINKER_LOG("MIOT Query Color");
            BlinkerMIOT.color(tempColor);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_BRIGHTNESS_NUMBER :
            BLINKER_LOG("MIOT Query Brightness");
            BlinkerMIOT.brightness(myBright);
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(button1state ? "on" : "off");
            BlinkerMIOT.color(tempColor);
            BlinkerMIOT.brightness(myBright);
            BlinkerMIOT.print();
            break;
    }
}

void switch_callback(const  String & state)
{ 
  tab1state = 1;
  if (state == BLINKER_CMD_ON){
    button1state = 1;
    FastLED.setBrightness(255);
    myHSVcolor.h = random8();
    FastLED.showColor(myHSVcolor);
    BUILTIN_SWITCH.print("on");
  } else{
    button1state = 0;
    FastLED.setBrightness(0);
    FastLED.show();
    BUILTIN_SWITCH.print("off"); 
  }
}

void heartbeat()
{
    if (FastLED.getBrightness() == 0 || (leds[0].r == 0 && leds[0].g == 0 && leds[0].b == 0)) {
      button1.print("off");
      rgb1.brightness(0);
    }else {
      button1.print("on");
    }

    if (button1state) BUILTIN_SWITCH.print("on");
    else BUILTIN_SWITCH.print("off");
}

void setup(){
  Serial.begin(115200);           //开启串口
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();
  Blinker.begin(auth, type);  //Blinker开始

  Blinker.attachNoButtonReset(noButtonReset); //专属设备没有实体按键重置
  button1.attach(button1_callback); //注册button1回调函数
  rgb1.attach(rgb1_callback);       //注册色盘回调函数
  tab1.attach(tab1_callback,  tab1_feedback); //注册标签回调函数2个
  slider1.attach(slider1_callback);   //注册滑动条回调函数
  BUILTIN_SWITCH.attach(switch_callback); //内置按钮 不用打开设备页面就可以控制设备
  Blinker.attachHeartbeat(heartbeat); //注册心跳包
  BlinkerMIOT.attachPowerState(miotPowerState); //注册小爱开关
  BlinkerMIOT.attachBrightness(miotBright); //注册小爱亮度
  BlinkerMIOT.attachColor(miotColor); //注册小爱颜色
  BlinkerMIOT.attachQuery(miotQuery); //注册小爱查询
  LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );  // 初始化光带  
  FastLED.setBrightness(0);        // 设置光带亮度
  FastLED.show();

  while(Blinker.status() != PRO_DEV_CONNECTED){
    if(Blinker.status() == PRO_WLAN_SMARTCONFIG_BEGIN) myRGBcolor = CRGB::Blue;
    else if(Blinker.status() == PRO_WLAN_APCONFIG_BEGIN) myRGBcolor = CRGB(0,255,255);
    else myRGBcolor = CRGB::Green;
    FastLED.setBrightness(255); 
    fill_solid(leds, NUM_LEDS,  myRGBcolor);
    FastLED.show();
    Blinker.delay(350);
    fill_solid(leds, NUM_LEDS,  CRGB::Black);
    FastLED.show();
    Blinker.delay(350);

    if (millis()> 90000 && (Blinker.status() == PRO_WLAN_CONNECTING || Blinker.status() == PRO_DEV_AUTHCHECK_FAIL || Blinker.status() == PRO_DEV_REGISTER_FAIL)) //如果系统运行时间大于1分钟并且是“设备连接网络中”状态，就重置配网信息，让系统进入AP配网状态
    {
      Blinker.reset();
    }
    if (millis() > 180000 && Blinker.status() == PRO_WLAN_SMARTCONFIG_BEGIN) //如果设备运行1.5分钟了，并且是“等待智能配网状态”，就变为AP配网模式；
    { 
      Blinker.apConfigInit(); //ap配网初始化
    }
  }
  
}

void loop(){
  if (currentTime != 0 && setTime != 0){
    if (millis() - currentTime >= setTime) {  
      button1state = 0;
      FastLED.setBrightness(0);
      FastLED.show();
      slider1.print(0);
      rgb1.brightness(0);
      rgb1.print(255,  255, 255, 0);
      button1.color("#000000");
      button1.print("off");

      setTime = 0;
      currentTime = 0;
    }
  }

  Blinker.run();
  if (button1state == 1){
    switch(tab1state){
      case 0:
        Breath();
        break; 
      case 1:
        break;
      case 2:
        if (changeIndex == 0) {
          Change0();
        }else if (changeIndex == 1){
          Change1();
        }else if (changeIndex == 2){
          Change2();
        }else if (changeIndex == 3){
          Change3();
        }else if (changeIndex == 4){
          Change4();
        }else if (changeIndex == 5){
          Change5();
        }
        break;
      case 3:
        SimulateColor();
        break;
      case 4:
        if (effectIndex == 0) {
          Effect0();
        }else if (effectIndex == 1){
          Effect1();
        }else if (effectIndex == 2){
          Effect2();
        }else if (effectIndex == 3){
          Effect3();
        }else if (effectIndex == 4){
          Effect4();
        }else if (effectIndex == 5){
          Effect5();
        }else if (effectIndex == 6){
          Effect6();
        }else if (effectIndex == 7){
          Effect7();
        }
        break;
      default:
        break;
    }
  }
  
}

//LavaColors_p   ForestColors_p OceanColors_p  HeatColors_p  CloudColors_p 
//RainbowColors_p  RainbowStripeColors_p   PartyColors_p

void SimulateColor(){  //仿色函数
  for(uint8_t i = 0;  i < 60; i++){
    beginColorIndex++;
    fill_palette (leds, NUM_LEDS, beginColorIndex, 1, myColorPalette[colorPaletteIndex], 255, LINEARBLEND );
    FastLED.show();
    delay(myBright/5 + 15);
  }
}



void Change0(){   //柔和渐变
  FastLED.setBrightness(myHSVcolor.v);
  for(uint8_t i = 0; i < 30; i++){
    myHSVcolor.h++;
    fill_solid(leds, NUM_LEDS, myHSVcolor);
    FastLED.show();
    delay(40);
  }
}
void Change1(){   //随机跳变
  FastLED.setBrightness(myHSVcolor.v);
  fill_solid(leds,  NUM_LEDS, myHSVcolor);
  FastLED.show();
  delay(1000);
  myHSVcolor.h = random8();
}

void Change2(){   //反色渐变
  FastLED.setBrightness(myHSVcolor.v);
  while (myHSVcolor.s != 0)
  {
    fill_solid(leds,  NUM_LEDS, myHSVcolor);
    FastLED.show();
    myHSVcolor.s--;
    delay(5);
  }
  myHSVcolor.h += 127;
  while (myHSVcolor.s != 255)
  {
    fill_solid(leds,  NUM_LEDS, myHSVcolor);
    FastLED.show();
    myHSVcolor.s++;   
    delay(5); 
  }
}
void Change3(){   //反色跳变
  FastLED.setBrightness(myHSVcolor.v);
  fill_solid(leds,  NUM_LEDS, myHSVcolor);
  FastLED.show();
  myHSVcolor.h += 127;
  delay(1000);
}


void Change4(){   //对比色渐变
  while (myHSVcolor.s != 0)
  {
    fill_solid(leds,  NUM_LEDS, myHSVcolor);
    FastLED.show();
    myHSVcolor.s--;
    delay(3);
  }
  myHSVcolor.h += 85;
  while (myHSVcolor.s != 255)
  {
    fill_solid(leds,  NUM_LEDS, myHSVcolor);
    FastLED.show();
    myHSVcolor.s++;   
    delay(3); 
  }
}
void Change5(){   //对比色跳变
  FastLED.setBrightness(myHSVcolor.v);
  fill_solid(leds,  NUM_LEDS, myHSVcolor);
  FastLED.show();
  myHSVcolor.h += 85;
  delay(1000);
}


void Breath (){      //呼吸函数
    if (tab1state != 0)return;  //如果tab标签不是0就跳出呼吸函数
    for (uint8_t b_brightness = 1; b_brightness < 255; b_brightness++)
    {
      FastLED.setBrightness(b_brightness);
      FastLED.show();
      delay(10);
    }
    Blinker.delay(300);
    if (tab1state != 0)return; 
    for (uint8_t b_brightness = 254; b_brightness > 0; b_brightness--)
    {
      FastLED.setBrightness(b_brightness);
      FastLED.show();
      delay(10);
    }
    Blinker.delay(700);
}

void Effect0(){   //8个渐灭珠子，不停的转圈圈 可变颜色 可改变延迟
  for(uint8_t j = 0; j < 16; j++){ 
    uint8_t vValue = 1;
    for(uint8_t i = 0; i < 8; i++){
      leds[(i+j)%16] = CHSV(myHSVcolor.h,255,vValue += 32);
    }
    FastLED.show();  
    Blinker.delay(90 - myBright/3); //90~5
    FastLED.clear(); 
  }
}

void Effect1(){ //交通信号灯
  FastLED.showColor(CRGB::Green);
  Blinker.delay(3000);
  if(effectIndex != 1 || tab1state != 4) return;
  for (uint8_t i = 0; i < 3; i++)
  {
    FastLED.showColor(CRGB::Green);
    Blinker.delay(375);
    FastLED.showColor(CRGB::Black);
    Blinker.delay(375);
  }
  if(effectIndex != 1 || tab1state != 4) return;
  FastLED.showColor(CRGB::Orange);
  Blinker.delay(1500);
  if(effectIndex != 1 || tab1state != 4) return;
  FastLED.showColor(CRGB::Red);
  Blinker.delay(3000);
  if(effectIndex != 1 || tab1state != 4) return;
}
void Effect2(){ //匪警车灯 110
  for (uint8_t i = 0; i < 15; i++)
  {
    FastLED.showColor(CRGB::Red);
    delay(25);
    FastLED.showColor(CRGB::Black);
    delay(25);
  }
  for (uint8_t i = 0; i < 15; i++)
  {
    FastLED.showColor(CRGB::Blue);
    delay(25);
    FastLED.showColor(CRGB::Black);
    delay(25);
  }
  
}

void Effect3()   //随机忽明忽暗 亮度条可调节明暗速度
{   
    fill_solid(leds, 16, myRGBcolor);
    uint8_t j = FastLED.getBrightness();
    uint8_t k = random8(255);
    if (j > k) 
    {
      for(uint8_t i = j-k; i > 0 ;i--){
        FastLED.setBrightness(j--);
        FastLED.show();
        delay(1+myBright/50);
      }
    }
    else
    {
      for(uint8_t i = k-j; i > 0 ;i--){
        FastLED.setBrightness(j++);
        FastLED.show();
        delay(1+myBright/50);
      }
    } 
    delay(random8(255));
  
}

void Effect4() //可调双色闪烁，亮度越大 闪烁越快
{
  FastLED.showColor(myRGBcolor);
  Blinker.delay(260-myBright);
  FastLED.showColor(myRGBcolorA);
  Blinker.delay(260-myBright);
}

void Effect5()  //星星闪烁 亮度越大 闪烁的星星越少
{
  if ( random8() <  260 - myBright )
  {
    leds[ random8(NUM_LEDS) ] = myRGBcolor;
  }
  FastLED.show();
  Blinker.delay(10);
  FastLED.clear();

}

void Effect6(){  //星星逐渐熄灭，亮度越大 闪烁的星星越少
  int pos = random8(NUM_LEDS);
  EVERY_N_MILLISECONDS( 100 ) { 
    if( random8() < 260 - myBright) {
      leds[pos] = myRGBcolor;
    }   
  }
  EVERY_N_MILLISECONDS( 10 ) { 
    fadeToBlackBy( leds, NUM_LEDS, 5);
  }    
  FastLED.show(); 
  delay(30); 
}

void Effect7(){ //双色不停转圈圈 亮度越大 转圈越快
  for (uint8_t i = 0; i < 16; i++)
  {
    leds[i] = myRGBcolorA;
    leds[(i+8)%16] = myRGBcolor;
    FastLED.show();
    delay(265-myBright);  //265 ~ 10
  }
}