





// return a struct with distance, and dregre is radians relative to the front of the robot 


// #include "mpu.h"
#include <Arduino.h>
#include "VL53_sensors.h"
// #include "VL53L5CX_sensor.h"
#include "VL53LCX.h"


VL53LCX front_sensor(15,4);
VL53_sensors sensores;


// 0 --> 90
// 1 --> 180
// 2 --> -90 

float vl_pos ;
float vl_dist;

void setup(){

  Serial.begin(112500);

  Serial.print(".");
  sensores.sensorsInit();
  delay(300);
  front_sensor.init();
  // VL53L5_init();
  delay(1000);

}


void loop(){
 
 sensores.distanceRead();
 
 front_sensor.update();

 float vl_pos = front_sensor.position;
 float vl_dist = front_sensor.distance;

 
 float enemy_positon[3] = {-PI/2,PI,PI/2};
 float enemy_pos = 0 ; 
 int treshold_distance = 150 ;

  
  for (uint8_t i = 0; i < 3; i++){

     
      if(sensores.dist[i] < treshold_distance){
         enemy_pos = enemy_positon[i] ;
      } 
      
  }

  Serial.print("V0: ");
  Serial.print(sensores.dist[0]);
  Serial.print(": V1: ");
  Serial.print(sensores.dist[1]);
  Serial.print(" V12 ");
  Serial.print(sensores.dist[2]);
  Serial.print(" | POS: ");
  Serial.print(enemy_pos);
  Serial.print(" VL_DIST: ");
  Serial.print(vl_dist);
  Serial.print(" VL_POS: ");
  Serial.println(vl_pos);

}   

