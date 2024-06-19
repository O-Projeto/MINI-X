





// return a struct with distance, and dregre is radians relative to the front of the robot 


// #include "mpu.h"
#include <Arduino.h>
#include "VL53_sensors.h"
#include "VL53L5CX_sensor.h"

VL53_sensors sensores;


// 0 --> 90
// 1 --> 180
// 2 --> -90 




void setup(){

  Serial.begin(112500);


  sensores.sensorsInit();
  delay(300);
  VL53L5_init();
  delay(1000);

}


void loop(){
 
 sensores.distanceRead();
 float vl_pos = VL53L5_get_position();
 float vl_dist = VL53L5_get_distance();

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

