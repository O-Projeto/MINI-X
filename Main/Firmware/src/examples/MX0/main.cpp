#include <Arduino.h>
#include "MX0.h"

MX0 razoes;

void setup(){   
    Serial.begin(112500);
    razoes.init();
}

void loop(){
    razoes.process();
    razoes.debug();
}

