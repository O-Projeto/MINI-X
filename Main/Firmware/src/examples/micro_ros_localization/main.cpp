#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/twist.h>
#include <geometry_msgs/msg/pose_stamped.h>

#include "VL53_sensors.h"

#include <micro_ros_utilities/type_utilities.h>
#include <micro_ros_utilities/string_utilities.h>

#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>

#include "keys.h"

#include "config.h"

#include "cinematic.h"

#include <ESP32Servo.h>
Servo motor_esquerda;
Servo motor_direita ;

#include <ESP32Encoder.h>
ESP32Encoder encoder_esquerda;
ESP32Encoder encoder_direita;

#include "odometry.h"
#include "mpu.h" 

VL53_sensors sensores;

bool _imu_connect; 
bool _connect = false;
float* imu_ypr ;

float linear = 0 ;
float angular = 0 ;
float speed_left = 0 ;
float speed_right = 0 ;



rcl_subscription_t subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rcl_timer_t timer;

extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

const int watch_dog_msec = 1000;
int last_control_msec = 0;

rcl_publisher_t distance_back_t;
std_msgs__msg__Int32 distance_back_m;

rcl_publisher_t pose_t; 
geometry_msgs__msg__PoseStamped pose_m;





#define RCCHECK(fn)              \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }
#define RCSOFTCHECK(fn)          \
  {                              \
    rcl_ret_t temp_rc = fn;      \
    if ((temp_rc != RCL_RET_OK)) \
    {                            \
      error_loop();              \
    }                            \
  }

void error_loop()
{
  while (1)
  {
    Serial.printf("error_loop\n");
    delay(1);
  }
}

// twist message cb
void subscription_callback(const void *msgin)
{
  const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
  Serial.printf("%f, %f\n", msg->linear.x, msg->angular.z);
  // last_control_msec = millis();

  angular = msg->angular.z; 
  linear = msg->linear.x;
  
  speed_left =  speed_converter(cinematic_left(angular,linear));
  speed_right = speed_converter(cinematic_right(angular,linear));
  // Serial.println(speed_left);
  motor_esquerda.write(speed_left); 
  motor_direita.write(speed_right); 
 

}

void setup()
{


//this order is important, 
  motor_direita.attach(pinEsc1); 
  motor_esquerda.attach(pinEsc2); 

  //dont remove this delay !!!
  delay(255);

  // init motors 
  motor_direita.writeMicroseconds(1500); 
  motor_esquerda.writeMicroseconds(1500); 


  ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN);
  encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN,ENCODER_RIGHT_B_PIN);

  _imu_connect = imu_setup();
  set_microros_wifi_transports(keys::wifi_ssid, keys::wifi_pass, keys::agent_ip, keys::agent_port);
  delay(2000);


 
  sensores.sensorsInit();



  Serial.begin(115220);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create subscriber ros2 run tf2_ros static_transform_publisher 0 0 0 00 00 00 map pose_frame


  RCCHECK(rclc_publisher_init_default(
  &pose_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, PoseStamped),
  "zero/pose"));
  RCCHECK(rclc_publisher_init_default(
  &distance_back_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/distance/back"));

  RCCHECK(rclc_subscription_init_default(
      &subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
      "cmd_vel"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));
}

void loop()
{ 


  // sensores.distanceRead();

  // struct timespec tv = {0};
  // clock_gettime(0, &tv);

  // if (millis() - last_control_msec > watch_dog_msec)
  // {
  //   Serial.println(".");
  // }
  //   imu_ypr = imu_get_ypr(); 
  //   odom(encoder_esquerda.getCount(), -1*encoder_direita.getCount(),imu_ypr[0]);
  
  //   double theta_half = th / 2.0;
  //   double sin_theta_half = sin(theta_half);
  //   double cos_theta_half = cos(theta_half);

    

  //   pose_m.pose.orientation.x = 0.0;
  //   pose_m.pose.orientation.y = 0.0;
  //   pose_m.pose.orientation.z = sin_theta_half;
  //   pose_m.pose.orientation.w = cos_theta_half;
    
  // pose_m.pose.position.x = x ; 
  // pose_m.pose.position.y = y; 



  // pose_m.header.stamp.sec = tv.tv_sec;
  // pose_m.header.stamp.nanosec = tv.tv_nsec;

  // pose_m.header.frame_id.size = 20;
  // pose_m.header.frame_id.data = "pose_frame";


  // distance_back_m.data =  sensores.dist[1];
  // RCSOFTCHECK(rcl_publish(&distance_back_t, &distance_back_m, NULL));

  



  

  // RCSOFTCHECK(rcl_publish(&pose_t, &pose_m, NULL));
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));
}
