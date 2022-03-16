#include <Arduino.h>

const unsigned char mychar[7] PROGMEM = {0,1,2,4,8,-8,0};

int cumulateSize(const unsigned char *mychara){
    
}

void setup(){
    Serial.begin(115200);
    Serial.println("");
    Serial.println(cumulateSize(mychar));
    Serial.println(sizeof(mychar));
    Serial.println(mychar[0]);
    Serial.println(mychar[1]);
    Serial.println(mychar[2]);
    Serial.println(mychar[3]);
    Serial.println(mychar[4]);
    Serial.println(mychar[5]);
    
}

void loop(){

}

