#include <Arduino.h>

// Parameters
double wheeltrack = 0.300;   // distance between wheels
double wheelradius = 0.075;  // radius of the wheel in meters
int TPR = 360;          // ticks per turn
double left_ticks = 0;
double right_ticks = 0;
double last_left_ticks = 0;
double last_right_ticks = 0;
double heading = 0;
double heading_offset = 0;
bool reset_odom = false;
long current_time; 
long last_time; 


// x = 0
double x = 0.0; // consider robot front not base_link
double y = 0.0;
double th = 0.0;

double vx = 0.0;
double vy = 0.0;
double vth = 0.0;


// return X,Y and theta 
struct robot_position {
     float x ;
     float y ; 
     float theta ; 
   
};





robot_position odom(int64_t pulseLeft,int64_t pulseRight, float yaw)
{
    robot_position robot; 


    double delta_L = pulseLeft - last_left_ticks;
    double delta_R = pulseRight - last_right_ticks;
    double dl = 2 * M_PI * wheelradius * delta_L / TPR;
    double dr = 2 * M_PI * wheelradius * delta_R / TPR;
    double dc = (dl + dr) / 2;
    double dth = (dr - dl) / wheeltrack;

    if (dr == dl)
    {
        double dx = dr * cos(th);
        double dy = dr * sin(th);
        x += dx;
        y += dy;
    }
    else
    {
        double radius = dc / dth;
        double iccX = x - radius * sin(th);
        double iccY = y + radius * cos(th);
        double dx = cos(dth) * (x - iccX) - sin(dth) * (y - iccY) + iccX - x;
        double dy = sin(dth) * (x - iccX) + cos(dth) * (y - iccY) + iccY - y;
        x += dx;
        y += dy;
    }

    // th = yaw - heading_offset;
    th = yaw;

    // if (reset_odom)
    // {
    //     x = 0;
    //     y = 0;
    //     heading_offset = yaw;
    // }

    // Serial.print("X: " );
    // Serial.print(x);
    // Serial.print(" Y: ");
    // Serial.print(y);
    // Serial.print(" Yaw: ");
    // Serial.print(th);
    // Serial.println("");

    last_left_ticks = pulseLeft;
    last_right_ticks = pulseRight;

    robot.x = x ; 
    robot.y = y ; 
    robot.theta = th ; 

    return robot; 

}