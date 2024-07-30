#include "kinematic.h"

#define L  0.09 
#define R 0.015

kinematic robo(32,33,L,R);
float K_circulo = 2;

void setup(){
    Serial.begin(115200);
    robo.init();
  

}

void loop(){

    robo.move(1,1*K_circulo);
    // robo.motor_left.write(1600);
    // robo.motor_right.write(1600);
    robo.debug();
    // delay(2000);
    Serial.println("");
}