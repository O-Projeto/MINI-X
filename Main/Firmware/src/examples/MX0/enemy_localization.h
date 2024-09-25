#include "VL53_sensors.h"
#include "VL53L5CX_sensor.h"

#define SDIST_1 25 //change the pins 
#define SDIST_2 23
#define SDIST_3 13

struct enemy_localization_cord {
    float angle;
    int dist;
};

// 0 --> 90
// 1 --> 180
// 2 --> -90
// 3 --> 0 // vl->fodao 

// return a struct with distance, and dregre is radians relative to the front of the robot 

#define N_SENSOR 3

int x_shut_pins[N_SENSOR] = {SDIST_1, SDIST_2, SDIST_3};

class Enemy_localization{
  private:
  // VL53_sensors sens_dist_lineares(N_SENSOR, x_shut_pins);
  VL53_sensors sens_dist_lineares;

  float max_pos;
  float mim_pos;
  float max_rad;
  float mim_rad;
  int numerovls = 3;
  int vlc = 0;  // variavel para silumar os vls para calibrar o PID
  
  // VL_Fodao vlf;
  enemy_localization_cord enemy_cord_info; 
  vl_data vl_data_storage; 
  int dist_fodao;
  
  public:
  int l = 0 ;
  float sens_pos_robot[4] =  {-90, 180, 90, 0}; //[rad]
  // float sens_pos_robot[3] =  {-90, 180, 0}; //[rad]
  // float sens_pos_robot[2] =  {-90, 180}; //[rad]

  float sens_dist_robot[3];

  Enemy_localization(){

  };
  void update_dist();
  void init_sensors(TwoWire &wire);
    // void init_sensors();

  enemy_localization_cord get_info();
  void debug();
};



void Enemy_localization::init_sensors(TwoWire &wire){
  // Serial.println("Começo do init dos vls");
  VL53L5_init();
  sens_dist_lineares.sensorsInit(wire);
  // Serial.println("Fim init dos vls");
}

void Enemy_localization::update_dist(){
  sens_dist_lineares.distanceRead();
  vl_data_storage = VL53L5_get_info();
  Serial.print(l++);
}

enemy_localization_cord Enemy_localization::get_info(){

  this->update_dist();
  sens_pos_robot[3] = vl_data_storage.pos;

  
  for (int i = 0; i < numerovls; i++){
      sens_dist_robot[i] = sens_dist_lineares.dist[i];
  }
  
  enemy_cord_info.dist = 10000;
  for (int i = 0; i < 3; i++){
    if ( enemy_cord_info.dist > sens_dist_robot[i]){
      enemy_cord_info.dist = sens_dist_robot[i];
      enemy_cord_info.angle = sens_pos_robot[i];
    }
  }
  return enemy_cord_info ;  
  
 
}

void Enemy_localization::debug(){
  
  Serial.print(" -0: ");
  Serial.print(sens_dist_robot[0]);  // -90 V

  Serial.print(" 1: ");
  Serial.print(sens_dist_robot[1]);  // 180 V

  Serial.print(" 2: ");
  Serial.print(sens_dist_robot[2]);  // 90 X

  Serial.print(" 3: ");
  Serial.print(sens_dist_robot[3]); // VL fodâo 0 graus X

  Serial.print(" | DIST: ");
  Serial.print( enemy_cord_info.dist); // melhor distancia vista pelos VLs

  Serial.print(" | ANGLE: ");
  Serial.print( enemy_cord_info.angle); // angulo do VL com menor distancia
  Serial.print(" | : ");

}