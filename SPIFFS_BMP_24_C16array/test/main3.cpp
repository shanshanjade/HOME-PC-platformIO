#include <Arduino.h>
#include <FS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif
#define PIN D3

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);
  
const uint16_t colors[] = {matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)};

String file_name = "/44.bmp";
String textString = "";
uint8_t bmpWidth, bmpHight, buqiwei;
uint16_t pix[1024];

void displayPix(){      //显示图片信息
  if(SPIFFS.exists(file_name)){
    Serial.print(file_name);
    Serial.println("文件找到啦！");
  } else {
    Serial.print(file_name);
    Serial.println("文件不存在！");
    return;
  }
  
  File myDataFile = SPIFFS.open(file_name, "r");
  
  myDataFile.seek(18,SeekSet);
  bmpWidth = myDataFile.peek();
  Serial.print("图片的的宽度为：");  Serial.print(bmpWidth); Serial.println("pix");
  
  myDataFile.seek(4,SeekCur);
  bmpHight = myDataFile.peek();
  bmpHight = 256-bmpHight;
  Serial.print("图片的的高度为：");  Serial.print(bmpHight); Serial.println("pix");

  buqiwei = 4 - bmpWidth*3%4; if(buqiwei = 4)buqiwei = 0;
  Serial.print("每行补齐位是：");  Serial.println(buqiwei);

  
  int pointer = 54;
  uint8_t R,G,B;
  
  for(uint16_t i = 0; i < bmpHight*bmpWidth; i++){ 
    //此函数需要每行补齐位为0，即每行的像素数为4的倍数；
    myDataFile.seek(pointer, SeekSet);B = myDataFile.peek();pointer++;
    myDataFile.seek(pointer, SeekSet);G = myDataFile.peek();pointer++;
    myDataFile.seek(pointer, SeekSet);R = myDataFile.peek();pointer++;
    Serial.print(R);Serial.print(";"); Serial.print(G);Serial.print(";"); Serial.print(B);Serial.println(";");
    pix[i] = ((uint16_t)(R & 0xF8) << 8) | ((uint16_t)(G & 0xFC) << 3) | (B >> 3);
    Serial.println(pix[i],HEX);
//      dataFile.seek(pointer, SeekSet);
//      Serial.println(dataFile.peek(),HEX);pointer++;
  }
  matrix.drawRGBBitmap(0,0,pix,bmpWidth,bmpHight);
  myDataFile.close();
}
//读取文件流

void displayTxt(){
  if(SPIFFS.exists(file_name)){
    Serial.print(file_name);
    Serial.println("文件找到啦！");
  } else {
    Serial.print(file_name);
    Serial.println("文件不存在！");
    return;
  }
  
  File myDataFile = SPIFFS.open(file_name, "r");
  for(int i=0; i<myDataFile.size(); i++){
    textString = textString + (char)myDataFile.read();
    Serial.print((char)myDataFile.read());       
  }
  matrix.print(textString);
  myDataFile.close();
}

void displayDir() {   
  Serial.println("显示文件列表");     //显示目录 "/" 单一个斜杠
  Dir dir = SPIFFS.openDir("/");  // 建立“目录”对象
  while (dir.next()) {  // dir.next()用于检查目录中是否还有“下一个文件”
    Serial.println(dir.fileName()); // 输出文件名
  }
}

void removeFile(){        //删除文件 
  file_name = file_name.substring(1);
      if (SPIFFS.remove(file_name)){
        Serial.print(file_name);
        Serial.println(" remove sucess");
      } else {
        Serial.print(file_name);
        Serial.println(" remove fail");
      }
}

void setup(){
  Serial.begin(115200);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);
  if(SPIFFS.begin()){
    Serial.println("");
    Serial.println("");
    Serial.println("SPIFFS文件系统开始");
  } else {
    Serial.println("SPIFFS文件系统开始启动失败");
  }
  
}

void loop(){
  if(Serial.available() > 0){
    file_name = Serial.readString();
    if (file_name == "dir"){
      displayDir();
    }else if(file_name.startsWith("//")){
      removeFile();
    } else {
      if(file_name.endsWith(".bmp")){
        Serial.println(file_name);
        displayPix();
      } else if (file_name.endsWith(".txt")){
        Serial.println(file_name);
        displayTxt();
      }
    }
  }
  matrix.fillScreen(0);
  matrix.setCursor(0,0);
  matrix.drawRGBBitmap(0,0,pix,bmpWidth,bmpHight);
  matrix.drawXBitmap(0,0,guoshan,32,64,0xF800);
  matrix.show();
  delay(100);
}

const uint8_t guoshan[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x70,0x00,0x00,
0x00,0x38,0x0C,0x0C,0x00,0x38,0xEF,0xFE,0x3F,0xFF,0xFE,0x1C,0x00,0x00,0x0E,0x18,
0x00,0x01,0x8E,0x38,0x07,0xFF,0xCE,0x30,0x07,0x03,0x8E,0x30,0x07,0x03,0x8E,0x60,
0x07,0x03,0x8E,0x60,0x07,0xFF,0x8E,0xC0,0x07,0x03,0x8E,0xC0,0x06,0x01,0x8E,0x60,
0x1F,0xFF,0xCE,0x60,0x00,0x03,0xCE,0x30,0x00,0x06,0x0E,0x38,0x00,0x3C,0x0E,0x1C,
0x00,0x3C,0x0E,0x1C,0x00,0x3B,0xEE,0x1C,0x03,0xFE,0x0E,0x0C,0x1F,0xF8,0x0E,0x0C,
0x1C,0x38,0x0F,0x1C,0x00,0x38,0x0F,0xFC,0x00,0x38,0x0E,0x78,0x00,0x38,0x0E,0x00,
0x03,0xF8,0x0E,0x00,0x00,0x78,0x0E,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,/*"郭",0*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x00,0x01,0xE0,0x00,0x00,
0x00,0xF0,0x00,0x30,0x00,0xF7,0xFF,0xFC,0x0C,0x70,0x00,0x38,0x0F,0x60,0x00,0x38,
0x0E,0x00,0x00,0x38,0x0E,0x01,0xC0,0x38,0x0E,0x01,0xE0,0x38,0x0E,0x01,0xC0,0x38,
0x0E,0x01,0xC0,0x38,0x0E,0x01,0xC0,0x38,0x0E,0x03,0x80,0x38,0x0E,0x03,0x80,0x38,
0x0E,0x03,0xC0,0x38,0x0E,0x03,0xF0,0x38,0x0E,0x07,0x38,0x38,0x0E,0x07,0x1E,0x38,
0x0E,0x0E,0x0F,0x38,0x0E,0x1C,0x0F,0x38,0x0E,0x18,0x07,0x38,0x0E,0x70,0x07,0x38,
0x0E,0xE0,0x00,0x38,0x0F,0x80,0x00,0x38,0x0E,0x00,0x00,0x38,0x0E,0x00,0x00,0x38,
0x0E,0x00,0x03,0xF8,0x0E,0x00,0x00,0xF0,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x00/*"闪",1*/
}