#include "VL53_sensors.h"
#include "VL53L5CX_sensor.h"

struct enemy_localization_cord {
    float angle;
    int dist;
};



// 0 --> 90
// 1 --> 180
// 2 --> -90
// 3 --> 0 // vl->fodao 

// return a struct with distance, and dregre is radians relative to the front of the robot 
class Enemy_localization{
  private:
  VL53_sensors sens_dist_lineares;
  float max_pos;
  float mim_pos;
  float max_rad;
  float mim_rad;
  
  // VL_Fodao vlf;
  enemy_localization_cord enemy_cord_info; 
  vl_data vl_data_storage; 
  int dist_fodao;
  
  public:
  float sens_pos_robot[4] =  {-PI/2,PI,PI/2,0}; //[rad]
  float sens_dist_robot[4];

  Enemy_localization(){

  };
  void update_dist();
  void init_sensors();
  enemy_localization_cord get_info();
  void debug();
};



void Enemy_localization::init_sensors(){
  // Serial.println("Começo do init dos vls");
  VL53L5_init();
  sens_dist_lineares.sensorsInit();
  // Serial.println("Fim init dos vls");
}

void Enemy_localization::update_dist(){
  //  sens_dist_lineares.distanceRead();
   vl_data_storage = VL53L5_get_info();
}

enemy_localization_cord Enemy_localization::get_info(){

  this->update_dist();
  // sens_pos_robot[3] = vl_data_storage.pos;

  for (int i = 0; i < 3; i++){
      sens_dist_robot[i] = sens_dist_lineares.dist[i];
  }
  
  sens_dist_robot[3] = vl_data_storage.dist; 
  
  enemy_cord_info.dist = 10000;
  for (int i = 0; i < 4; i++){
    if ( enemy_cord_info.dist > sens_dist_robot[i]){
      enemy_cord_info.dist = sens_dist_robot[i];
      enemy_cord_info.angle = sens_pos_robot[i];
    }
  }
    return enemy_cord_info ;  
}

void Enemy_localization::debug(){
  Serial.print(" -0: ");
  Serial.print(sens_dist_robot[0]);
  Serial.print(" 1: ");
  Serial.print(sens_dist_robot[1]);
  Serial.print(" 2: ");
  Serial.print(sens_dist_robot[2]);
  Serial.print(" 3: ");
  Serial.print(sens_dist_robot[3]);

  Serial.print(" | DIST: ");
  Serial.print( enemy_cord_info.dist);

  Serial.print(" | 0: ");
  Serial.print( enemy_cord_info.angle);
  Serial.print(" | : ");



}