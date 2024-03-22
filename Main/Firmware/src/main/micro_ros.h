#include <Arduino.h>

#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include "config.h"
#include <std_msgs/msg/int32.h>
#include <geometry_msgs/msg/twist.h>


#include "keys.h"


rcl_subscription_t cmd_vel_subscriber;
geometry_msgs__msg__Twist msg;
rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;



rcl_publisher_t distance_left_t;
std_msgs__msg__Int32 distance_left_m;






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



  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &cmd_vel_subscriber, &msg, &subscription_callback, ON_NEW_DATA));
}


void ros_loop(float distance_left){

              distance_left_m.data =  distance_left;
              RCSOFTCHECK(rcl_publish(&distance_left_t, &distance_left_m, NULL));


              }