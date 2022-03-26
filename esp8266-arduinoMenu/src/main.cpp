#include <FS.h>
#include <menu.h>
#include <menuIO/serialIO.h>
<<<<<<< HEAD
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
=======
#include <SPIFFSMenu.h>
using namespace Menu;


//function to handle file select
// declared here and implemented bellow because we need
// to give it as event handler for `filePickMenu`
// and we also need to refer to `filePickMenu` inside the function
result filePick(eventMask event, navNode& nav, prompt &item);


// SDMenu filePickMenu("SD Card","/",filePick,enterEvent);
//caching 32 file entries
// SDMenu filePickMenu("SPIFFS","/",filePick,enterEvent);
CachedSDMenu<32> filePickMenu("SPIFFS","/",filePick,enterEvent);
//implementing the handler here after filePick is defined...
result filePick(eventMask event, navNode& nav, prompt &item) {
  // switch(event) {//for now events are filtered only for enter, so we dont need this checking
  // case enterCmd:
      if (nav.root->navFocus==(navTarget*)&filePickMenu) {
        Serial.println();
        Serial.print("selected file:");
        Serial.println(filePickMenu.selectedFile);
        Serial.print("from folder:");
        Serial.println(filePickMenu.selectedFolder);
      }
  //     break;
  // }
  return proceed;
}
>>>>>>> 1bffa0a9eb01776c7b5088e16176b05bef0fafda

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