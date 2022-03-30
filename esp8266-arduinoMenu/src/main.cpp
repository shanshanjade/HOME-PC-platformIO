#include <FS.h>
#include <menu.h>
#include <menuIO/serialIO.h>
#include <plugin/SPIFFSMenu.h>
using namespace Menu;

class altPrompt:public prompt {
public:
  // altPrompt(constMEM promptShadow& p):prompt(p) {}
  using prompt::prompt;
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);
  }
};


#define MAX_DEPTH 2
int test = 50;
MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
    ,FIELD(test,"TEST","%",0,100,10,1,doNothing,noEvent,wrapStyle)

    ,EXIT("退出")

) ;
MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

void setup(){
    Serial.begin(112500);
    while(!Serial);
    Serial.println("PxMatrix Menu Set");
    nav.useAccel=false;
}
void loop(){
    nav.poll();
    delay(100);
}