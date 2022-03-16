#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <FS.h>
#define PIN D3
#define NUMPIXELS 256
Adafruit_NeoPixel pixels(NUMPIXELS,PIN,NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix *matrix =new Adafruit_NeoMatrix(32, 8, D3,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

uint8_t bitmap[] = {B1111,B0111,B1101,B1110};
void setup() {
  Serial.begin(115200);
  matrix->begin();
  if(SPIFFS.begin()){
    Serial.println("");
    Serial.println("");
    Serial.println("SPIFFS文件系统开始");
  } else {
    Serial.println("SPIFFS文件系统开始启动失败");
  }
  matrix->setBrightness(10);
  matrix->setTextColor(matrix->Color(255,255,0));
}
void loop() {
  matrix->fillScreen(0);
  matrix->setCursor(0,0);
  matrix->print(F("GUOSHAN"));
  matrix->show();
  delay(20);
  
}