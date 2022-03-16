#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <FS.h>

#define PIN D3
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

File file;

String fileName;

uint16_t pixel[1024];

// int readByte();
// int readWord();
// long readDword();
// void decodeBMP();
// void showBMP();

int8_t G_biWidth;
int8_t G_biHeight;

int transitionY = 0;
int8_t transitionFlag = -1;

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println("SPIFFS started!");
  matrix.begin();
  matrix.setBrightness(5);
}
void loop() {
  if(Serial.available() > 0){
    fileName = Serial.readString();
    Serial.println(fileName);
    decodeBMP(fileName);
  }
  showBMP();
}
void showBMP(){
  //ws2812显示图片
  matrix.fillScreen(0);
  matrix.drawRGBBitmap(0,transitionY,pixel,G_biWidth,G_biHeight);
  matrix.show();
  transitionY += transitionFlag; if(transitionY < -24 || transitionY > 0) transitionFlag = -transitionFlag;
  delay(20);
}

int readByte(){
  return file.read();
}
int readWord(){
  int b0 = readByte();
  int b1 = readByte();
  return (b1 << 8) | b0;
}
long readDword(){
  long b0 = readByte();
  long b1 = readByte();
  long b2 = readByte();
  long b3 = readByte();
  return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}
void decodeBMP(String _fileName){
  int bfType; //说明文件的类型，一般为 42 4D BM
  int bfSize; //该位图文件的大小，用字节为单位
  int bfReserved1;//保留，必须设置为0
  int bfReserved2;//保留，必须设置为0
  int bfOffBits;  //即从文件头到位图数据需偏移****字节。
  int biSize; 
  int biWidth;//说明像素的宽度
  int biHeight;//说明像素的高度 如果要想得到一个正值，则要每位取反；
  bool isRevOrder;// *自添加值* 如果像素的高度是个正数，说明图像时倒向的，如果是个负数，则说明是正向的位图，大多数的bmp文件都是倒向的位图
  int biPlanes; //为目标设备说明颜色平面数；其值总是1；
  int biBitCount; //说明图片的位数；1、4、8、16、24、32位
  int biCompression;//说明图像数据压缩的类型，一般都为0
  int biSizeImage; //说明图像的大小，当biCompression为0的时候，可以设置为0；
  int biXPelsPerMeter;//说明水平分辨率，用像素/每米表示，有符号整数；
  int biYPelsPerMeter;//说明垂直分辨率，用像素/每米表示，有符号整数；
  int biColorUsed;//位图中实际使用的颜色表中的颜色索引数（设为0的话，说明使用所有调色板选项）
  int biColorImportant;//说明对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要
  bool isIndexColor; //*自添加值* 如果biColorUsed和biColorImportant都为0，则说明不使用色板没有索引颜色
  int skipByte;//*自添加值* 补齐位：如果水平像素数为4的倍数，则不需要补齐位
  file = SPIFFS.open(_fileName,"r");
  if(file == -1){
    Serial.println(F("未找到文件！"));
    return;
  }
  bfType = readWord();
  if(bfType != 19778){
    Serial.println(F("不是bmp文件！"));
    return;
  }
  Serial.print(F("文件类型为："));Serial.println(bfType);//19778 BM
  bfSize = readDword();
  Serial.print(F("文件大小为："));Serial.println(bfSize);
  bfReserved1 = readWord();
  Serial.print(F("保留位1："));Serial.println(bfReserved1);
  bfReserved2 = readWord();
  Serial.print(F("保留位2："));Serial.println(bfReserved2);
  bfOffBits = readDword();
  Serial.print(F("偏移量："));Serial.println(bfOffBits);
  biSize = readDword();
  Serial.print(F("头结构的字节数："));Serial.println(biSize);
  biWidth = readDword();
  G_biWidth = biWidth;
  Serial.print(F("图像宽度："));Serial.println(biWidth);
  biHeight = readDword();
  G_biHeight = biHeight;
  Serial.print(F("图像高度："));Serial.println(biHeight);
  if (biHeight < 0 ) {
    isRevOrder = true;
    Serial.println(F("图像是正向序列，非正常序列，但读取数据时好用"));
  }else {
    isRevOrder = false;
    Serial.println(F("图像是倒向序列，为正常序列"));
  }
  biPlanes = readWord();
  Serial.print(F("颜色平面数："));Serial.println(biPlanes);
  biBitCount = readWord();
  Serial.print(F("图像位数："));Serial.println(biBitCount);
  biCompression = readDword();
  Serial.print(F("图像压缩类型："));Serial.println(biCompression);
  biSizeImage = readDword();
  Serial.print(F("图像的大小为："));Serial.println(biSizeImage);
  biXPelsPerMeter = readDword();
  Serial.print(F("图像水平分辨率为（像素/米）："));Serial.println(biXPelsPerMeter);
  biYPelsPerMeter = readDword();
  Serial.print(F("图像垂直分辨率为（像素/米）："));Serial.println(biYPelsPerMeter);
  biColorUsed = readDword();
  Serial.print(F("索引颜色数为："));Serial.println(biColorUsed);
  biColorImportant = readDword();
  Serial.print(F("重要的索引颜色数为："));Serial.println(biColorImportant);
  if (biColorUsed == 0 && biColorImportant == 0){
    isIndexColor = false;
    Serial.println(F("图像不是索引颜色，没有色板"));
  }else{
    Serial.println(F("图像是索引色"));
  }
  skipByte =4 - ((biBitCount * biHeight * 4 / 32) % 4);if (skipByte == 4) skipByte = 0;
  Serial.print(F("图像每行的补齐位是："));Serial.println(skipByte);
  //***************************************************************************
  //开始解析数据部分 解析的是16位的（565RGB）bmp图像
  if ( isRevOrder ){
    Serial.println(F("开始正向序列读取数据"));
    for (uint8_t i = 0; i < biHeight; i++){
      for (uint8_t j = 0; i < biWidth; j++){
        pixel[i * biWidth + j] = readWord();
        Serial.println(F("解析中..."));
      }
      //跳过补齐位
      if (skipByte != 0){
        for (uint8_t k = 0; k < skipByte; k++){
          readByte();
        }
      }
    }  
  } else {
    Serial.println(F("开始倒向序列读取数据"));
    for (uint8_t i = biHeight - 1; i >= 0; i--){
      for (uint8_t j = 0; j < biWidth; j++){
        pixel[i * biWidth + j] = readWord();
        Serial.println(F("解析中..."));
      }
      //跳过补齐位
      if (skipByte != 0){
        for (uint8_t k = 0; k < skipByte; k++){
          readByte();
        }
      }
    }
  }
  file.close();
  Serial.println("关闭文件流");
}














