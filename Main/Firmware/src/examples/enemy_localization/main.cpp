


// #include "mpu.h"
#include <Arduino.h>

 
#include "enemy_localization.h"

enemy_localization_cord enemy_info;



// 0 --> 90
// 1 --> 180
// 2 --> -90


Enemy_localization enemy;


void setup(){

  Serial.begin(112500);
  enemy.init_sensors();

 
}


void loop(){

  enemy_info = enemy.get_info();
  enemy.debug();

  Serial.println();

 

}