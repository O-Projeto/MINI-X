
//get the encoder, mpu, and line sensor input and tranforns in a position 

#include "mpu.h"
#include "odometry.h"
#include <ESP32Encoder.h>

#include<Arduino.h>

#define ENCODER_LEFT_A_PIN 19
#define ENCODER_LEFT_B_PIN 18

#define ENCODER_RIGHT_A_PIN 27
#define ENCODER_RIGHT_B_PIN 14



class localization
{
private:
    bool mpu_connected = false ; 
    float* imu_ypr;
    int64_t pulseLeft, pulseRight;
    ESP32Encoder encoder_esquerda;
    ESP32Encoder encoder_direita;
    robot_position robot; 
    /* data */
public:
    localization(/* args */);
    robot_position getPosition(); 
    void init();
    
    ~localization();
};

localization::localization(/* args */)
{
    // ESP32Encoder::useInternalWeakPullResistors=UP;
    
    encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN);
    encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN,ENCODER_RIGHT_B_PIN);

}

void localization::init(){

    mpu_connected = imu_setup();
}

localization::~localization()
{
}


robot_position localization::getPosition(){
    imu_ypr = imu_get_ypr();  
    pulseRight =  -1*encoder_direita.getCount();
    pulseLeft = encoder_esquerda.getCount() ;

    robot = odom(pulseLeft,pulseRight,imu_ypr[0]);

    return robot ; 
}


 