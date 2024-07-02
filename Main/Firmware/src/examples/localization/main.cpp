#include "localization.h"

localization robot_pos(); 

robot_position robot;  

void setup(){

    Serial.begin(115200);

}

void loop(){
    robot = robot_pos.get_position();

    Serial.print("X: ");
    Serial.print(robot.x); 
    Serial.print(" Y: ");
    Serial.print(robot.y);
    Serial.print(" Th: ");
    Serial.print(robot.theta);

    Serial.println();

    
    

}