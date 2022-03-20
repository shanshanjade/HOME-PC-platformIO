#include <Arduino.h>
#include <PxMatrix.h>
#include <Config.h>
#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>
#include <menuIO/adafruitGfxOut.h>
using namespace Menu;

#define BLACK       0x0000      /*    0,   0,   0 */
#define NAVY        0x000F      /*    0,   0, 128 */
#define DARKGREEN   0x03E0      /*    0, 128,   0 */
#define DARKCYAN    0x03EF      /*    0, 128, 128 */
#define MAROON      0x7800      /*  128,   0,   0 */
#define PURPLE      0x780F      /*  128,   0, 128 */
#define OLIVE       0x7BE0      /*  128, 128,   0 */
#define LIGHTGREY   0xC618      /*  128, 128,   0 */
#define GRAY        0xC618      /*  192, 192, 192 */
#define DARKGRAY    0x7BEF      /*  128, 128, 128 */
#define BLUE        0x001F      /*    0,   0, 255 */
#define GREEN       0x07E0      /*    0, 255,   0 */
#define CYAN        0x07FF      /*    0, 255, 255 */
#define RED         0xF800      /*  255,   0,   0 */
#define MAGENTA     0xF81F      /*  255,   0, 255 */
#define YELLOW      0xFFE0      /*  255, 255,   0 */
#define WHITE       0xFFFF      /*  255, 255, 255 */
#define ORANGE      0xFDA0      /*  255, 180,   0 */
#define GREENYELLOW 0xB7E0      /*  180, 255,   0 */
#define PINK        0xFC9F      
const colorDef<uint16_t> colors[6] MEMMODE={
  {{BLACK,BLACK},{BLACK,DARKGREEN,ORANGE}},//bgColor
  {{GRAY,GRAY},{WHITE,WHITE,WHITE}},//fgColor
  {{WHITE,BLACK},{YELLOW,YELLOW,RED}},//valColor
  {{WHITE,BLACK},{WHITE,YELLOW,YELLOW}},//unitColor
  {{WHITE,GRAY},{BLACK,DARKGREEN,WHITE}},//cursorColor
  {{WHITE,YELLOW},{DARKCYAN,WHITE,WHITE}},//titleColor
};

int test=55;
int ledCtrl=LOW;
result alert(menuOut& o,idleEvent e);
result doAlert(eventMask e, prompt &item);
TOGGLE(ledCtrl,setLed,"LED: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("ON",HIGH,doNothing,noEvent)
  ,VALUE("OFF",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,noTitle
  ,FIELD(test,"TEST:","%",0,255,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(setLed)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  ,OP("Alert test",doAlert,enterEvent)
  ,EDIT("HEX",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("Exit")
);

#define gfxWidth 66
#define gfxHeight 32
#define fontX 6
//5
#define fontY 8
#define MAX_DEPTH 2

serialIn serial(Serial);
#define textScale 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(display,colors,fontX,fontY,{0,0,gfxWidth/fontX,gfxHeight/fontY})
  ,SERIAL_OUT(Serial)
);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

result alert(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.setCursor(0,0);
    o.print(F("alert test"));
    o.setCursor(0,1);
    o.print(F("press [select]"));
    o.setCursor(0,2);
    o.print(F("to continue..."));
  }
  return proceed;
}

result doAlert(eventMask e, prompt &item) {
  nav.idleOn(alert);
  return proceed;
}
//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  o.setCursor(0,0);
  o.print(F("suspended..."));
  o.setCursor(0,1);
  o.print(F("press [select]"));
  o.setCursor(0,2);
  o.print(F("to continue"));
  return proceed;
}
//------显示相关设置--------------------------------
uint8_t display_draw_time = 15;  //30-70 is usually fine // display_draw_time:10 brightness:128
void displayTask(void* displayTask) {
    while (1) {
        display.display(display_draw_time);
        vTaskDelay(1);  //此处是延时1ms
    }
}
void init_Display() {
    display.begin(16);
    display.setBrightness(128);
    display.setColorOffset(0, 0, 100);  //best (0, 0, 110)
    display.setTextColor(0x07E0);
    display.setTextWrap(false);
    display.setFastUpdate(true);
    display.clearDisplay();
    xTaskCreatePinnedToCore(displayTask, "displayTask", 1000, NULL, 2, NULL, 1);  // PRO_CPU 为 0, APP_CPU 为 1 4096
};

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("menu 4.x test"));
  Serial.flush();
  init_Display();
  nav.idleTask=idle;
  // nav.showTitle=false;
}

void loop() {
  
  nav.poll();
  delay(100);
}