#include "kinematic.h"

#define L  0.09 
#define R 0.015

kinematic robo(32,33,L,R);

void setup(){
    Serial.begin(115200);
    robo.init();
  

}

void loop(){

    robo.move(0.5,0);
    // robo.motor_left.write(1600);
    // robo.motor_right.write(1600);
    robo.debug();
    // delay(2000);
    Serial.println("");
}