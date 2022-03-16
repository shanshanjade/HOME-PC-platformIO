#include <Arduino.h>
// #define LED_BUILTIN D0
void setup(){
    pinMode(LED_BUILTIN,OUTPUT);
}

void loop(){
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); //初始化引脚LED_BUILTIN
  delay(1000);
}