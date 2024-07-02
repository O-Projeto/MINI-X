
//get the encoder, mpu, and line sensor input and tranforns in a position 

#include "mpu.h"
#include "odometry.h"
#include <ESP32Encoder.h>



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
    robot_position get_position(); 
    
    ~localization();
};

localization::localization(/* args */)
{
    mpu_connected = imu_setup();
    ESP32Encoder::useInternalWeakPullResistors=UP;
    
    encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN);
    encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN,ENCODER_RIGHT_B_PIN);

}

localization::~localization()
{
}


robot_position localization::get_position(){
    imu_ypr = imu_get_ypr();  
    pulseRight =  -1*encoder_direita.getCount();
    pulseLeft = encoder_esquerda.getCount() ;

    robot = odom(pulseRight,pulseLeft,imu_ypr[0]);

    return robot ; 
}


 