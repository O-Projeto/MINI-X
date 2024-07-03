
//integrate the enemy and robot localization process and output to the kinematics 
// input -> robot_localization 
// input -> enemy_localizatio 
//output -> linear and angular speed 
#include "localization.h"
#include "config.h"



struct robot_speed {
     float linear ;
     float angular ; 
};


class MX0
{
private:
    localization robot_localization;
    robot_position robot_pos;
    

public:
    MX0(/* args */);
    void init();
    robot_speed process();
    ~MX0();
};


void MX0::init(){
     robot_localization.init();

}
robot_speed MX0::process(){
   robot_pos = robot_localization.getPosition();


}




MX0::~MX0()
{
}
