#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>

#define PIN D3


const uint8_t ApcFont35Bitmaps[] PROGMEM = {
    0x00,                               /* 0x20 space */
    0x80, 0x80, 0x80, 0x00, 0x80,       /* 0x21 exclam */
    0xA0, 0xA0,                         /* 0x22 quotedbl */
    0xA0, 0xE0, 0xA0, 0xE0, 0xA0,       /* 0x23 numbersign */
    0x60, 0xC0, 0x60, 0xC0, 0x40,       /* 0x24 dollar */
    0x80, 0x20, 0x40, 0x80, 0x20,       /* 0x25 percent */
    0xC0, 0xC0, 0xE0, 0xA0, 0x60,       /* 0x26 ampersand */
    0x80, 0x80,                         /* 0x27 quotesingle */
    0x40, 0x80, 0x80, 0x80, 0x40,       /* 0x28 parenleft */
    0x80, 0x40, 0x40, 0x40, 0x80,       /* 0x29 parenright */
    0xA0, 0x40, 0xA0,                   /* 0x2A asterisk */
    0x40, 0xE0, 0x40,                   /* 0x2B plus */
    0x40, 0x80,                         /* 0x2C comma */
    0xE0,                               /* 0x2D hyphen */
    0x80,                               /* 0x2E period */
    0x20, 0x20, 0x40, 0x80, 0x80,       /* 0x2F slash */
    0xE0, 0xA0, 0xA0, 0xA0, 0xE0,       /* 0x30 zero */
    0x40, 0x40, 0x40, 0x40, 0x40,       /* 0x31 one */
    0xE0, 0x20, 0xE0, 0x80, 0xE0,       /* 0x32 two */
    0xE0, 0x20, 0xE0, 0x20, 0xE0,       /* 0x33 three */
    0xA0, 0xA0, 0xE0, 0x20, 0x20,       /* 0x34 four */
    0xE0, 0x80, 0xE0, 0x20, 0xE0,       /* 0x35 five */
    0xE0, 0x80, 0xE0, 0xA0, 0xE0,       /* 0x36 six */
    0xE0, 0x20, 0x20, 0x20, 0x20,       /* 0x37 seven */
    0xE0, 0xA0, 0xE0, 0xA0, 0xE0,       /* 0x38 eight */
    0xE0, 0xA0, 0xE0, 0x20, 0xE0,       /* 0x39 nine */
    0x80, 0x00, 0x80,                   /* 0x3A colon */
    0x40, 0x00, 0x40, 0x80,             /* 0x3B semicolon */
    0x20, 0x40, 0x80, 0x40, 0x20,       /* 0x3C less */
    0xE0, 0x00, 0xE0,                   /* 0x3D equal */
    0x80, 0x40, 0x20, 0x40, 0x80,       /* 0x3E greater */
    0xE0, 0x20, 0x40, 0x00, 0x40,       /* 0x3F question */
    0x40, 0xA0, 0xE0, 0x80, 0x60,       /* 0x40 at */
    0x40, 0xA0, 0xE0, 0xA0, 0xA0,       /* 0x41 A */
    0xC0, 0xA0, 0xC0, 0xA0, 0xC0,       /* 0x42 B */
    0x60, 0x80, 0x80, 0x80, 0x60,       /* 0x43 C */
    0xC0, 0xA0, 0xA0, 0xA0, 0xC0,       /* 0x44 D */
    0xE0, 0x80, 0xE0, 0x80, 0xE0,       /* 0x45 E */
    0xE0, 0x80, 0xE0, 0x80, 0x80,       /* 0x46 F */
    0x60, 0x80, 0xE0, 0xA0, 0x60,       /* 0x47 G */
    0xA0, 0xA0, 0xE0, 0xA0, 0xA0,       /* 0x48 H */
    0xE0, 0x40, 0x40, 0x40, 0xE0,       /* 0x49 I */
    0x20, 0x20, 0x20, 0xA0, 0x40,       /* 0x4A J */
    0xA0, 0xA0, 0xC0, 0xA0, 0xA0,       /* 0x4B K */
    0x80, 0x80, 0x80, 0x80, 0xE0,       /* 0x4C L */
    0xA0, 0xE0, 0xE0, 0xA0, 0xA0,       /* 0x4D M */
    0xA0, 0xE0, 0xE0, 0xE0, 0xA0,       /* 0x4E N */
    0x40, 0xA0, 0xA0, 0xA0, 0x40,       /* 0x4F O */
    0xC0, 0xA0, 0xC0, 0x80, 0x80,       /* 0x50 P */
    0x40, 0xA0, 0xA0, 0xE0, 0x60,       /* 0x51 Q */
    0xC0, 0xA0, 0xE0, 0xC0, 0xA0,       /* 0x52 R */
    0x60, 0x80, 0x40, 0x20, 0xC0,       /* 0x53 S */
    0xE0, 0x40, 0x40, 0x40, 0x40,       /* 0x54 T */
    0xA0, 0xA0, 0xA0, 0xA0, 0x60,       /* 0x55 U */
    0xA0, 0xA0, 0xA0, 0x40, 0x40,       /* 0x56 V */
    0xA0, 0xA0, 0xE0, 0xE0, 0xA0,       /* 0x57 W */
    0xA0, 0xA0, 0x40, 0xA0, 0xA0,       /* 0x58 X */
    0xA0, 0xA0, 0x40, 0x40, 0x40,       /* 0x59 Y */
    0xE0, 0x20, 0x40, 0x80, 0xE0,       /* 0x5A Z */
    0xE0, 0x80, 0x80, 0x80, 0xE0,       /* 0x5B bracketleft */
    0x80, 0x40, 0x20,                   /* 0x5C backslash */
    0xE0, 0x20, 0x20, 0x20, 0xE0,       /* 0x5D bracketright */
    0x40, 0xA0,                         /* 0x5E asciicircum */
    0xE0,                               /* 0x5F underscore */
    0x80, 0x40,                         /* 0x60 grave */
    0xC0, 0x60, 0xA0, 0xE0,             /* 0x61 a */
    0x80, 0xC0, 0xA0, 0xA0, 0xC0,       /* 0x62 b */
    0x60, 0x80, 0x80, 0x60,             /* 0x63 c */
    0x20, 0x60, 0xA0, 0xA0, 0x60,       /* 0x64 d */
    0x60, 0xA0, 0xC0, 0x60,             /* 0x65 e */
    0x20, 0x40, 0xE0, 0x40, 0x40,       /* 0x66 f */
    0x60, 0xA0, 0xE0, 0x20, 0x40,       /* 0x67 g */
    0x80, 0xC0, 0xA0, 0xA0, 0xA0,       /* 0x68 h */
    0x80, 0x00, 0x80, 0x80, 0x80,       /* 0x69 i */
    0x20, 0x00, 0x20, 0x20, 0xA0, 0x40, /* 0x6A j */
    0x80, 0xA0, 0xC0, 0xC0, 0xA0,       /* 0x6B k */
    0xC0, 0x40, 0x40, 0x40, 0xE0,       /* 0x6C l */
    0xE0, 0xE0, 0xE0, 0xA0,             /* 0x6D m */
    0xC0, 0xA0, 0xA0, 0xA0,             /* 0x6E n */
    0x40, 0xA0, 0xA0, 0x40,             /* 0x6F o */
    0xC0, 0xA0, 0xA0, 0xC0, 0x80,       /* 0x70 p */
    0x60, 0xA0, 0xA0, 0x60, 0x20,       /* 0x71 q */
    0x60, 0x80, 0x80, 0x80,             /* 0x72 r */
    0x60, 0xC0, 0x60, 0xC0,             /* 0x73 s */
    0x40, 0xE0, 0x40, 0x40, 0x60,       /* 0x74 t */
    0xA0, 0xA0, 0xA0, 0x60,             /* 0x75 u */
    0xA0, 0xA0, 0xE0, 0x40,             /* 0x76 v */
    0xA0, 0xE0, 0xE0, 0xE0,             /* 0x77 w */
    0xA0, 0x40, 0x40, 0xA0,             /* 0x78 x */
    0xA0, 0xA0, 0x60, 0x20, 0x40,       /* 0x79 y */
    0xE0, 0x60, 0xC0, 0xE0,             /* 0x7A z */
    0x60, 0x40, 0x80, 0x40, 0x60,       /* 0x7B braceleft */
    0x80, 0x80, 0x00, 0x80, 0x80,       /* 0x7C bar */
    0xC0, 0x40, 0x20, 0x40, 0xC0,       /* 0x7D braceright */
    0x60, 0xC0,                         /* 0x7E asciitilde */
};
/* {offset, width, height, advance cursor, x offset, y offset} */
const GFXglyph ApcFont35Glyphs[] PROGMEM = {
    {0, 8, 1, 2, 0, -5},   /* 0x20 space */
    {1, 8, 5, 2, 0, -5},   /* 0x21 exclam */
    {6, 8, 2, 4, 0, -5},   /* 0x22 quotedbl */
    {8, 8, 5, 4, 0, -5},   /* 0x23 numbersign */
    {13, 8, 5, 4, 0, -5},  /* 0x24 dollar */
    {18, 8, 5, 4, 0, -5},  /* 0x25 percent */
    {23, 8, 5, 4, 0, -5},  /* 0x26 ampersand */
    {28, 8, 2, 2, 0, -5},  /* 0x27 quotesingle */
    {30, 8, 5, 3, 0, -5},  /* 0x28 parenleft */
    {35, 8, 5, 3, 0, -5},  /* 0x29 parenright */
    {40, 8, 3, 4, 0, -5},  /* 0x2A asterisk */
    {43, 8, 3, 4, 0, -4},  /* 0x2B plus */
    {46, 8, 2, 3, 0, -2},  /* 0x2C comma */
    {48, 8, 1, 4, 0, -3},  /* 0x2D hyphen */
    {49, 8, 1, 2, 0, -1},  /* 0x2E period */
    {50, 8, 5, 4, 0, -5},  /* 0x2F slash */
    {55, 8, 5, 4, 0, -5},  /* 0x30 zero */
    {60, 8, 5, 3, 0, -5},  /* 0x31 one */
    {65, 8, 5, 4, 0, -5},  /* 0x32 two */
    {70, 8, 5, 4, 0, -5},  /* 0x33 three */
    {75, 8, 5, 4, 0, -5},  /* 0x34 four */
    {80, 8, 5, 4, 0, -5},  /* 0x35 five */
    {85, 8, 5, 4, 0, -5},  /* 0x36 six */
    {90, 8, 5, 4, 0, -5},  /* 0x37 seven */
    {95, 8, 5, 4, 0, -5},  /* 0x38 eight */
    {100, 8, 5, 4, 0, -5}, /* 0x39 nine */
    {105, 8, 3, 2, 0, -4}, /* 0x3A colon */
    {108, 8, 4, 3, 0, -4}, /* 0x3B semicolon */
    {112, 8, 5, 4, 0, -5}, /* 0x3C less */
    {117, 8, 3, 4, 0, -4}, /* 0x3D equal */
    {120, 8, 5, 4, 0, -5}, /* 0x3E greater */
    {125, 8, 5, 4, 0, -5}, /* 0x3F question */
    {130, 8, 5, 4, 0, -5}, /* 0x40 at */
    {135, 8, 5, 4, 0, -5}, /* 0x41 A */
    {140, 8, 5, 4, 0, -5}, /* 0x42 B */
    {145, 8, 5, 4, 0, -5}, /* 0x43 C */
    {150, 8, 5, 4, 0, -5}, /* 0x44 D */
    {155, 8, 5, 4, 0, -5}, /* 0x45 E */
    {160, 8, 5, 4, 0, -5}, /* 0x46 F */
    {165, 8, 5, 4, 0, -5}, /* 0x47 G */
    {170, 8, 5, 4, 0, -5}, /* 0x48 H */
    {175, 8, 5, 4, 0, -5}, /* 0x49 I */
    {180, 8, 5, 4, 0, -5}, /* 0x4A J */
    {185, 8, 5, 4, 0, -5}, /* 0x4B K */
    {190, 8, 5, 4, 0, -5}, /* 0x4C L */
    {195, 8, 5, 4, 0, -5}, /* 0x4D M */
    {200, 8, 5, 4, 0, -5}, /* 0x4E N */
    {205, 8, 5, 4, 0, -5}, /* 0x4F O */
    {210, 8, 5, 4, 0, -5}, /* 0x50 P */
    {215, 8, 5, 4, 0, -5}, /* 0x51 Q */
    {220, 8, 5, 4, 0, -5}, /* 0x52 R */
    {225, 8, 5, 4, 0, -5}, /* 0x53 S */
    {230, 8, 5, 4, 0, -5}, /* 0x54 T */
    {235, 8, 5, 4, 0, -5}, /* 0x55 U */
    {240, 8, 5, 4, 0, -5}, /* 0x56 V */
    {245, 8, 5, 4, 0, -5}, /* 0x57 W */
    {250, 8, 5, 4, 0, -5}, /* 0x58 X */
    {255, 8, 5, 4, 0, -5}, /* 0x59 Y */
    {260, 8, 5, 4, 0, -5}, /* 0x5A Z */
    {265, 8, 5, 4, 0, -5}, /* 0x5B bracketleft */
    {270, 8, 3, 4, 0, -4}, /* 0x5C backslash */
    {273, 8, 5, 4, 0, -5}, /* 0x5D bracketright */
    {278, 8, 2, 4, 0, -5}, /* 0x5E asciicircum */
    {280, 8, 1, 4, 0, -1}, /* 0x5F underscore */
    {281, 8, 2, 3, 0, -5}, /* 0x60 grave */
    {283, 8, 4, 4, 0, -4}, /* 0x61 a */
    {287, 8, 5, 4, 0, -5}, /* 0x62 b */
    {292, 8, 4, 4, 0, -4}, /* 0x63 c */
    {296, 8, 5, 4, 0, -5}, /* 0x64 d */
    {301, 8, 4, 4, 0, -4}, /* 0x65 e */
    {305, 8, 5, 4, 0, -5}, /* 0x66 f */
    {310, 8, 5, 4, 0, -4}, /* 0x67 g */
    {315, 8, 5, 4, 0, -5}, /* 0x68 h */
    {320, 8, 5, 2, 0, -5}, /* 0x69 i */
    {325, 8, 6, 4, 0, -5}, /* 0x6A j */
    {331, 8, 5, 4, 0, -5}, /* 0x6B k */
    {336, 8, 5, 4, 0, -5}, /* 0x6C l */
    {341, 8, 4, 4, 0, -4}, /* 0x6D m */
    {345, 8, 4, 4, 0, -4}, /* 0x6E n */
    {349, 8, 4, 4, 0, -4}, /* 0x6F o */
    {353, 8, 5, 4, 0, -4}, /* 0x70 p */
    {358, 8, 5, 4, 0, -4}, /* 0x71 q */
    {363, 8, 4, 4, 0, -4}, /* 0x72 r */
    {367, 8, 4, 4, 0, -4}, /* 0x73 s */
    {371, 8, 5, 4, 0, -5}, /* 0x74 t */
    {376, 8, 4, 4, 0, -4}, /* 0x75 u */
    {380, 8, 4, 4, 0, -4}, /* 0x76 v */
    {384, 8, 4, 4, 0, -4}, /* 0x77 w */
    {388, 8, 4, 4, 0, -4}, /* 0x78 x */
    {392, 8, 5, 4, 0, -4}, /* 0x79 y */
    {397, 8, 4, 4, 0, -4}, /* 0x7A z */
    {401, 8, 5, 4, 0, -5}, /* 0x7B braceleft */
    {406, 8, 5, 2, 0, -5}, /* 0x7C bar */
    {411, 8, 5, 4, 0, -5}, /* 0x7D braceright */
    {416, 8, 2, 4, 0, -5}, /* 0x7E asciitilde */
};
const GFXfont ApcFont35 PROGMEM = {(uint8_t *)ApcFont35Bitmaps,(GFXglyph *)ApcFont35Glyphs, 0x20, 0x7E, 6};

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
const uint16_t colors[] = { 
  matrix.Color(255, 0, 0), 
  matrix.Color(255, 255, 0),
  matrix.Color(0, 255, 0), 
  matrix.Color(0, 255, 255), 
  matrix.Color(0, 0, 255),
  matrix.Color(255, 0, 255)
  };

Ticker textScroll;
Ticker matrixShow;

int cursorX = 0;
unsigned int oneSecond = 0; 
int flag = -1;

void textTicker_callback(char *message){
  matrix.clear();
  oneSecond++;
  matrix.setCursor(cursorX,6);
  matrix.print(message);
  if (oneSecond > 40 && strlen(message)*4 > 31) cursorX+=flag;
  if(cursorX < -((int)strlen(message)*4 -31) || cursorX > 1){
    flag = -flag;
  }
}

void matrixShow_callback(){ matrix.show();}
void showTEXT(char* message,uint16_t color){
  flag = -1;
  cursorX = 0;
  oneSecond = 0;
  String my = message;
  matrix.setTextColor(color);
  if(textScroll.active()){
    textScroll.detach();
  }
  textScroll.attach(0.04, textTicker_callback,message);
}

String ipaddr;



char*menuL0[] = {"TEXT COLOR","CLOCK COLOR","BRIGHTNESS","GIF BMP DELAY"};
char*menuL1[4][3] = {
  {"SYSTEM COLOR","MESSAGE COLOR"},
  {"TIME COLOR","DATE COLOR","WEEK COLOR"},
  {"MIN-BRIGHTNESS","MAX-BRIGHTNESS"},
  {"VAL:"}
};

//--------------------------------------------------------------
int systemColorIndex = 0;
typedef struct keycodeStruct{
  uint8_t keyStateIndex;
  uint8_t keyDownState;
  uint8_t keyUpState;
  uint8_t keyEnterState;
  uint8_t keyEscState;
  char*   menuString;
  void (*operation)();
};
void menu41(){Serial.println(systemColorIndex);Serial.println("menu41"); systemColorIndex++; if(systemColorIndex > 5)systemColorIndex=0;   };
void empty(){};
keycodeStruct myMenu[12] =  {
  //序 下 上 右 左
  { 0, 1, 0, 4, 0,">text color",*empty},
  { 1, 2, 1, 6, 1,">clock color",*empty},
  { 2, 3, 1, 11,2,">brightness",*empty}, 
  { 3, 3, 2, 11,3,">delay",*empty},

  {4,5,4,11,0,">>system color",*empty},
  {5,5,4,11,0,">>message color",*empty},

  {6,7,6,11,1,">>date color",*empty},
  {7,8,6,11,1,">>time color",*empty},
  {8,8,7,11,1,">>week color",*empty},

  {9,10,9,11,2,">>minbrightness",*empty},
  {10,10,9,11,2,">>maxbrightness",*empty},

  {11,11,0,11,0,">>>value",*menu41}
};
//--------------------------------------------------------
void setup(){
  Serial.begin(115200);
  delay(1000);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[1]);
  matrix.setFont(&ApcFont35);
  matrix.setCursor(0,6);
  String myString = "  :ARTRIX";
  matrix.print(myString);
  matrix.show();
  delay(5000);

  matrixShow.attach_ms(50,matrixShow_callback);
  WiFi.begin("xiaomi123","a.13513710972");
  showTEXT("CONNECT TO xiaomi123",colors[0]);
  delay(1000);
  showTEXT("CONNECTING...",colors[1]);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  showTEXT("WIFI CONNECTED!",colors[2]);
  delay(2000);
  showTEXT("ENJOY:)",colors[0]);
  delay(2000);
  ipaddr = "IP:" + WiFi.localIP().toString();
  Serial.println(ipaddr);
  showTEXT((char *)ipaddr.c_str(),colors[2]);
}
char *tempMenu = menuL0[0];

void loop(){
  if(Serial.available() > 0){
    int keyvalue = Serial.parseInt();
    static int funcIndex = 0;
    switch (keyvalue){
      case 2:
        funcIndex = myMenu[funcIndex].keyDownState;
        break;
      case 8:
        funcIndex = myMenu[funcIndex].keyUpState;
        break;
      case 6:
        funcIndex = myMenu[funcIndex].keyEnterState;
        break;
      case 4:
        funcIndex = myMenu[funcIndex].keyEscState;
        break;
    }
    Serial.println(myMenu[funcIndex].menuString);
    Serial.println(systemColorIndex);
    (*myMenu[funcIndex].operation)();
    showTEXT(myMenu[funcIndex].menuString,colors[systemColorIndex]);
  }
}
