/*#include <Arduino.h>

#include "Juiz.h"
#include "kinematic.h"

int pincontro = 34;
int pinled    = 26;
int numcor    = 4;
int numled    = 5;
int tempo     = 100;
int sinal     = -1;

Juiz Mx(pincontro, pinled, numled, numcor, tempo);

#define L  0.09 
#define R 0.015

kinematic robo(32,33,L,R);

void setup(){
  Serial.begin(115200);
  robo.init();
  Mx.setupjuiz();
}

void loop() { 
  sinal = Mx.comando();
  
  Serial.print(" MCR: ");
  Serial.print(sinal);
  if (sinal == 1){
    robo.move(0,0);
    // Serial.print(sinal);
    Serial.print("   Alerta!!!");
  }
  if (sinal == 2){
    robo.move(0.5,0);
    // Serial.print(sinal);
    Serial.print("   Modo BATALHAAA");
  }
  if (sinal == 3){
    robo.move(0,0);
    // Serial.print(sinal);
    Serial.print("   STOP");
  }

  Serial.println("");
  }
*/







// return a struct with distance, and dregre is radians relative to the front of the robot 


// #include "mpu.h"
#include <Arduino.h>
#include "VL53_sensors.h"
#include "VL_Fodao.h"

VL53_sensors sensores;

VL_Fodao vlf(15, 4);

// 0 --> 90
// 1 --> 180
// 2 --> -90

void setup(){

  Serial.begin(112500);


  sensores.sensorsInit();
  delay(300);
  // vlf.init();
  delay(1000);

}


void loop(){
   sensores.distanceRead();


  for (uint8_t i = 0; i < 3; i++){

    Serial.print(" ");
    Serial.print(String(i));
    Serial.print(" ");
    Serial.print(sensores.dist[i]);

  }
    Serial.println("\t\t");
  delay(150);
}  
  /*for(int i = 0; i > 3; i++){
    Serial.print(sensorDistances[i]);
    Serial.print(" -=- ");
  }*/

  // loop vl fodao
  /*float p = vlf.VL53L5_get_distance();
  float d = vlf.VL53L5_get_distance();

  Serial.print(p);
  Serial.print("  =  ");
  Serial.println(d);*/

 /*sensores.distanceRead();
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
 */
