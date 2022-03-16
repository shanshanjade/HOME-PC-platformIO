#include "FastLED.h"            // 此示例程序需要使用FastLED库
 
#define NUM_LEDS 16             // LED灯珠数量
#define DATA_PIN D3              // Arduino输出控制信号引脚
#define LED_TYPE WS2812         // LED灯带型号
#define COLOR_ORDER GRB         // RGB灯珠中红色、绿色、蓝色LED的排列顺序
 
uint8_t max_bright = 255;       // LED亮度控制变量，可使用数值为 0 ～ 255， 数值越大则光带亮度越高
 
CRGB leds[NUM_LEDS];            // 建立光带leds
 
void setup() { 
  Serial.begin(9600);           // 启动串行通讯
  delay(1000);                  // 稳定性等待
  LEDS.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);  // 初始化光带
  FastLED.setBrightness(max_bright);                            // 设置光带亮度
  fill_solid(leds, 16 ,CRGB::Red);

}
 
void loop() {
    uint8_t j = FastLED.getBrightness();
    uint8_t k = random8(255);
    if (j > k) 
    {
      for(uint8_t i = j-k; i > 0 ;i--){
        FastLED.setBrightness(j--);
        FastLED.show();
        delay(3);
      }
    }
    else
    {
      for(uint8_t i = k-j; i > 0 ;i--){
        FastLED.setBrightness(j++);
        FastLED.show();
        delay(3);
      }
    } 
    delay(random8(255));
    // FastLED.setBrightness(random8(255));
    // FastLED.show();
    // FastLED.delay(4*random8(255));
}