
//integrate the enemy and robot localization process and output to the kinematics 
// input -> robot_localization 
// input -> enemy_localizatio 
//output -> linear and angular speed 
#include "localization.h"
#include "enemy_localization.h"
#include "config.h"
#include <Arduino.h>



struct robot_speed {
     float linear ;
     float angular ; 
};


class MX0
{
private:
    
    localization robot_localization;
    // current robot position
    robot_position robot_pos;
    robot_speed emocoes ;
    enemy_localization_cord enemy_info;
    Enemy_localization enemy;

    

public:
    MX0(/* args */);
    void init();
    robot_speed process();
    void debug();
   
};

MX0::MX0(){

}

void MX0::init(){
     Wire.begin();
     Wire.setClock(400000);
     enemy.init_sensors(Wire);
    //  delay(10000);
     robot_localization.init(Wire);
    //  delay(10000);

}
robot_speed MX0::process(){
   robot_pos = robot_localization.getPosition();
   enemy_info = enemy.get_info();
  emocoes.linear = 0 ;
  emocoes.angular = 0 ;
return emocoes ;


}

void MX0::debug(){
   robot_localization.debug();
   enemy.debug();
   Serial.println();
}



