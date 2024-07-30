
//integrate the enemy and robot localization process and output to the kinematics 
// input -> robot_localization 
// input -> enemy_localizatio 
//output -> linear and angular speed 
#include "localization.h"
#include "enemy_localization.h"
#include "config.h"
#include <Arduino.h>
#include "controller.h"
#include "controller.h" //PID

// =-=-=-=-=-=-=-= PID do GUI =-=-=-=-=-=-=-=
// float KP = -1.0 ; //constante correção de erros PID
// float KI = -0.001;
// float KD = -0.0000;

float KP = 0.9; //constante correção de erros PID
float KI = 0.1;
float KD = 0;

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
    Controller balancer_controller; 

    

    

public:
    MX0(/* args */);
    void init();
    robot_speed process();
    void debug();
   
};

MX0::MX0():balancer_controller(KP, KI, KD){

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
   emocoes.angular = balancer_controller.output(robot_pos.theta + enemy_info.angle,robot_pos.theta);
   emocoes.linear = 3/(10000);
   if(enemy_info.angle == 0 and enemy_info.dist < 500){
      emocoes.linear = 2;
   }
   if(enemy_info.angle == PI and enemy_info.dist < 500){
      emocoes.linear = -2;
   }
   else{
     emocoes.linear =  3/(10000);
    
   }
//    emocoes.linear = 0 ;

return emocoes ;


}

void MX0::debug(){
   robot_localization.debug();
   enemy.debug();
//    Serial.println();
}



