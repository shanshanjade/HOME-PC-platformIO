#include <Arduino.h>
// #include "zm16X16.h"
// #include "zm32X32.h"
void setup() {
  Serial.begin(115200);
  char beiUTF[] = "啊";
  Serial.println("UTF-8码（3位）：");
  Serial.print(beiUTF[0],HEX);
  Serial.print(beiUTF[1],HEX);
  Serial.println(beiUTF[2],HEX);
  unsigned int beiUC; 
  Serial.println("Unicode码（2位）：");
  beiUC = (beiUTF[0]&0x0F)<<12 | (beiUTF[1]&0b00111111)<<7 | (beiUTF[2]&0b00111111);
  Serial.println((beiUTF[0]&0x0F)<<12,HEX);
  Serial.println((beiUTF[1]&0b00111111)<<7,HEX);
  Serial.println((beiUTF[2]&0b00111111),HEX);
  
  Serial.println(beiUC,HEX);
}

void loop() {

//  if(Serial.available() >0){ 
//    String str1 = Serial.readString();
//    str1 += "mimi";
//    char cArr[str1.length() + 1];
//    char cArr2[str1.length() + 3];
//    str1.toCharArray(cArr,str1.length() + 1);
//    str1.toCharArray(cArr2,str1.length() + 3);
//    Serial.println(str1);
//    Serial.println(cArr);
//    Serial.println(cArr2);
//    Serial.println(sizeof(str1));
//    Serial.println(sizeof(cArr));
//    Serial.println(sizeof(cArr2));
//    Serial.println(strlen(cArr));
//    Serial.println(strlen(cArr2));
//    if(cArr[str1.length()] == '\0')
//    {
//      Serial.println("cArr1: has ending mark. ");
//    }
//    if(cArr2[str1.length()] == '\0')
//    {
//      Serial.println("cArr2: has ending mark. ");
//    }
//    Utf8ToUnicode(cArr,cArr2);
//  }
}

static char Utf8ToUnicode(char* utf8, char *unicode)
{
    char *pchar = utf8;
    int nBytes = 0;
    if (0 == (*utf8 & 0x80)) 
    {
        /*
         * single-byte char
         */
        nBytes = 1;
        unicode[0] = *utf8;
        Serial.println("不是个汉字");
    }
    else
    {
        /*
         * 3-byte char (chinese char)
         */
        int i;
        if ( (*utf8 & 0xf0) == 0xe0 ) 
        {
            nBytes  = 3;
            unicode[0] = ((utf8[0] & 0x0f) <<4) + ((utf8[1] & 0x3c) >>2);
            unicode[1] = ((utf8[1] & 0x03) <<6) + (utf8[2] & 0x3f);
            Serial.println("是个汉字");
            Serial.println(unicode[0],HEX);
            Serial.println(unicode[1],HEX);
        }
        else
        {
            Serial.println("ERROR: utf-8 to unicode, nBytes !=3\n");
            nBytes = 0;
            unicode[0] = '?';
            return 0;
        }
    }
    return nBytes;
}