#define BLINKER_PRO_ESP
#define BLINKER_NO_BUTTON
#define BLINKER_OTA_VERSION_CODE "0.1.1"
#include <FastLED.h>
#include <Blinker.h>
#define NUM_LEDS 120             // LED灯珠数量
#define DATA_PIN D3              // Arduino输出控制信号引脚
#define LED_TYPE WS2812         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
uint8_t myBright = 255;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
CRGB leds[NUM_LEDS];            // 建立光带leds对象
CRGB myRGBcolor(0,  0,  0);     //建立我的RGB颜色
CHSV myHSVcolor(0,  255,  255); //建立我的HSV颜色
CRGBPalette16 myColorPalette[] = {RainbowColors_p, RainbowStripeColors_p, PartyColors_p, LavaColors_p, ForestColors_p, OceanColors_p, HeatColors_p,CloudColors_p};
uint8_t colorPaletteIndex = 0;    //仿色函数使用变量 色板序号    做8个效果
uint8_t beginColorIndex = 0;       //仿色函数使用变量 开始颜色  做动画
uint8_t colorPaletteDelay = 0;    //仿色函数使用变量 延迟时间  用滑条做效果
uint8_t effectIndex = 0;          //特效函数使用变量 效果函数索引
uint8_t changeIndex = 0;          //变色函数使用变量 效果函数索引 用色盘做开始颜色
char type[] = "SbMDXt3ErelB";
char auth[] = "8oa5xXAFWHla";

void Effect0();
void Effect1();
void Effect2();
void Effect3();

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
    myRGBcolor.r = random8();//以上语句将返回0-255之间的随机数值。
    myRGBcolor.g = random8();//以上语句将返回0-255之间的随机数值。
    myRGBcolor.b = random8();//以上语句将返回0-255之间的随机数值。
    fill_solid(leds,  NUM_LEDS, myRGBcolor);
    FastLED.setBrightness(255);
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
  fill_gradient()
}
void rgb1_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value){
  if(button1state == 1){
    myRGBcolor.r = r_value;
    myRGBcolor.g = g_value;
    myRGBcolor.b = b_value;
      switch (tab1state){
          case 0:       //呼吸模式下只变颜色
            fill_solid(leds,  NUM_LEDS, myRGBcolor); 
            break;
          case 1:       //单色模式下变颜色和亮度
            fill_solid(leds,  NUM_LEDS, myRGBcolor);
            FastLED.setBrightness(bright_value);
            break;
          case 2:       //变色模式下改变颜色和亮度
            myHSVcolor = rgb2hsv_approximate(myRGBcolor);
            myHSVcolor.s = 255;
            myHSVcolor.v = bright_value;
            break;
          case 3:       //仿色模式下 用亮度改变延迟时间
            colorPaletteDelay = bright_value/5 + 10;
            colorPaletteDelay = map(colorPaletteDelay, 10, 60,  60, 10);
            break;
          case 4:       //特效模式下什么也不做
            break;
          default:
            break;
     }
    FastLED.show();
    if(bright_value == 0){
      button1state = 0;
      button1.color("#000000");
      button1.print("off");
    }else {
      button1state = 1;
      button1.color("#FF0000");
      button1.print("on");
    }
  }
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
              if(effectIndex > 3){effectIndex = 0;}
              break;
          default:
              break;
          }
    }
}
void tab1_feedback(){}

void heartbeat()
{
    if (FastLED.getBrightness() == 0 || (leds[0].r == 0 && leds[0].g == 0 && leds[0].b == 0)) 
    {
      button1.print("off");
      rgb1.brightness(0);
    }else 
    {
      button1.print("on");
    }  
}

void setup(){
  Serial.begin(115200);           //开启串口
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();
  Blinker.begin(auth, type);      //Blinker开始
  Blinker.attachNoButtonReset(noButtonReset);
  button1.attach(button1_callback); //注册button1回调函数
  rgb1.attach(rgb1_callback);       //注册色盘回调函数
  tab1.attach(tab1_callback,  tab1_feedback); //注册标签回调函数2个
  Blinker.attachHeartbeat(heartbeat); //注册心跳包
  LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );  // 初始化光带  
  FastLED.setBrightness(0);        // 设置光带亮度
  FastLED.show();
  while(Blinker.status() != PRO_DEV_CONNECTED){
    FastLED.setBrightness(255); 
    fill_solid(leds, NUM_LEDS,  CRGB::Red);
    FastLED.show();
    Blinker.delay(500);
    fill_solid(leds, NUM_LEDS,  CRGB::Black);
    FastLED.show();
    Blinker.delay(500);
  }
}

void loop(){
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
    fill_palette (leds, NUM_LEDS, beginColorIndex, 3, myColorPalette[colorPaletteIndex], myBright, LINEARBLEND );
    FastLED.show();
    delay(colorPaletteDelay);
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
void Change1(){   //彩虹跳变
  FastLED.setBrightness(myHSVcolor.v);
  fill_solid(leds,  NUM_LEDS, myHSVcolor);
  FastLED.show();
  delay(1000);
  myHSVcolor.h += 21;
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
    if (tab1state != 0){ return;  } //如果tab标签不是0就跳出呼吸函数
    for (uint8_t b_brightness = 1; b_brightness < 255; b_brightness++)
    {
      FastLED.setBrightness(b_brightness);
      FastLED.show();
      delay(10);
    }
    Blinker.delay(300);//Blinker.delay用于接收数据
    if (tab1state != 0){ return;  }
    for (uint8_t b_brightness = 254; b_brightness > 0; b_brightness--)
    {
      FastLED.setBrightness(b_brightness);
      FastLED.show();
      delay(10);
    }
    Blinker.delay(700);//Blinker.delay用于接收数据
}

void Effect0(){
  for(uint8_t i = 0; i < 8; i++){
    leds[i] = CHSV(0,255,255);
    leds[i+8] = CHSV(127,255,255);
  }
  FastLED.show();
  Blinker.delay(1000);
  for(uint8_t i = 0; i < 8; i++){
    leds[i] = CHSV(127,255,255);
    leds[i+8] = CHSV(0,255,255);
  }
  FastLED.show();
  Blinker.delay(1000);
}

void Effect1(){
  fill_rainbow(leds, 16, 0, 5);
  FastLED.show();
  Blinker.delay(1000);
}
void Effect2(){
  Serial.println("Effect2还没写好");
}

void Effect3()    //火焰函数
{
  bool gReverseDirection = false;
  FastLED.setBrightness(128);
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, (550 / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < 120 ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
    FastLED.show(); // display this frame
    FastLED.delay(17);
}
