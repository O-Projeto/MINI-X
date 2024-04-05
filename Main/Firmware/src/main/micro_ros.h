#include <Arduino.h>

#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include "config.h"
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/int64.h>
#include <geometry_msgs/msg/twist.h>
#include <sensor_msgs/msg/imu.h>

#include "keys.h"


rcl_subscription_t cmd_vel_subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;



rcl_publisher_t distance_left_t;
std_msgs__msg__Int32 distance_left_m;

rcl_publisher_t distance_right_t;
std_msgs__msg__Int32 distance_right_m;

rcl_publisher_t distance_back_t;
std_msgs__msg__Int32 distance_back_m;

rcl_publisher_t qre_front_t;
std_msgs__msg__Int32 qre_front_m;

rcl_publisher_t qre_back_t;
std_msgs__msg__Int32 qre_back_m;

rcl_publisher_t infra_t;
std_msgs__msg__Int32 infra_m;

rcl_publisher_t encoder_left_ticks_t;
std_msgs__msg__Int64 encoder_left_ticks_m;

rcl_publisher_t encoder_right_ticks_t;
std_msgs__msg__Int64 encoder_right_ticks_m;

rcl_publisher_t imu_publisher;
sensor_msgs__msg__Imu imu_msg;

float speed_linear_ros ;
float speed_angular_ros ; 

#define LED_PIN 2

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}


void error_loop(){
  for (int i = 0; i<10; i++)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
  ESP.restart();
}

//twist message cb
void subscription_callback(const void *msgin) {
  const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;
  // if velocity in x direction is 0 turn off LED, if 1 turn on LED
  speed_linear_ros = msg->linear.x ; 
  speed_angular_ros = msg->angular.z;
//   digitalWrite(LED_PIN, (msg->linear.x == 0) ? LOW : HIGH);
}

float getLinear(){
    return speed_linear_ros;
}

float getAngular(){
    return speed_angular_ros;
}


void init_ros(){

  set_microros_wifi_transports(keys::wifi_ssid, keys::wifi_pass, keys::agent_ip, keys::agent_port);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  
  delay(2000);

  allocator = rcl_get_default_allocator();

   //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_power_node", "", &support));

  RCCHECK(rclc_subscription_init_default(
      &cmd_vel_subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
      "cmd_vel"));

  RCCHECK(rclc_publisher_init_default(
  &distance_left_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/distance/left"));

  RCCHECK(rclc_publisher_init_default(
  &distance_back_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/distance/back"));

  RCCHECK(rclc_publisher_init_default(
  &distance_right_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/distance/right"));

  RCCHECK(rclc_publisher_init_default(
  &qre_front_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/qre/front"));

  RCCHECK(rclc_publisher_init_default(
  &qre_back_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/qre/back"));

  RCCHECK(rclc_publisher_init_default(
  &infra_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
  "zero/infra"));


  RCCHECK(rclc_publisher_init_default(
  &encoder_left_ticks_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int64),
  "zero/encoder/left/ticks"));

  RCCHECK(rclc_publisher_init_default(
  &encoder_right_ticks_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int64),
  "zero/encoder/right/ticks"));

  RCCHECK(rclc_publisher_init_best_effort(
    &imu_publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
   "zero/imu"));



  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &msg, &subscription_callback, ON_NEW_DATA));
}


void ros_loop(float distance_left,float distance_right,float distance_back, int qre_front, int qre_back, int infra, int encoder_left_ticks,int encoder_right_ticks,
              float *orientation, float *angular_velocity, float *linear_acceleration,float *orientationCovariance,float *linaerAccelerationCovariance,float *angularVelocityCovariance ){

              distance_left_m.data =  distance_left;
              RCSOFTCHECK(rcl_publish(&distance_left_t, &distance_left_m, NULL));

              distance_back_m.data =  distance_back;
              RCSOFTCHECK(rcl_publish(&distance_back_t, &distance_back_m, NULL));

              distance_right_m.data =  distance_right;
              RCSOFTCHECK(rcl_publish(&distance_right_t, &distance_right_m, NULL));

              qre_front_m.data =  qre_front;
              RCSOFTCHECK(rcl_publish(&qre_front_t, &qre_front_m, NULL));

              qre_back_m.data =  qre_back;
              RCSOFTCHECK(rcl_publish(&qre_back_t, &qre_back_m, NULL));

              infra_m.data =  infra;
              RCSOFTCHECK(rcl_publish(&infra_t, &infra_m, NULL));

              encoder_left_ticks_m.data =  encoder_left_ticks;
              RCSOFTCHECK(rcl_publish(&encoder_left_ticks_t, &encoder_left_ticks_m, NULL));

              encoder_right_ticks_m.data =  encoder_right_ticks;
              RCSOFTCHECK(rcl_publish(&encoder_right_ticks_t, &encoder_right_ticks_m, NULL));

              //TO DO 
              //cmd 
              // mpu  
              // odom 
              // bat 
              // pid parameters 
              imu_msg.header.frame_id.data = "imu_link";
              imu_msg.header.frame_id.size = 10;

              imu_msg.orientation.x = orientation[0];
              imu_msg.orientation.y = orientation[1];
              imu_msg.orientation.z = orientation[2];
              imu_msg.orientation.w = orientation[3]; 

            

              imu_msg.angular_velocity.x = angular_velocity[0];
              imu_msg.angular_velocity.y = angular_velocity[1];
              imu_msg.angular_velocity.z = angular_velocity[2];

              imu_msg.linear_acceleration.x = linear_acceleration[0]; 
              imu_msg.linear_acceleration.y = linear_acceleration[1]; 
              imu_msg.linear_acceleration.z = linear_acceleration[2]; 

                  for (int i = 0; i < 9; i ++) {
                    imu_msg.orientation_covariance[i] = orientationCovariance[i];
                    imu_msg.angular_velocity_covariance[i] =angularVelocityCovariance[i]; 
                    imu_msg.linear_acceleration_covariance[i] = linaerAccelerationCovariance[i];
                } 




              RCSOFTCHECK(rmw_uros_sync_session(1000));

              int64_t time = rmw_uros_epoch_millis();

              imu_msg.header.stamp.sec = time;

              // Publish IMU data
              RCSOFTCHECK(rcl_publish(&imu_publisher, &imu_msg, NULL));

              }