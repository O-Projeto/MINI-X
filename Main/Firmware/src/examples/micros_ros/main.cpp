#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/twist.h>


#include "keys.h"

#include "config.h"

#include "cinematic.h"

#include <ESP32Servo.h>
Servo motor_esquerda;
Servo motor_direita ;

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

const int watch_dog_msec = 1000;
int last_control_msec = 0;

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
  last_control_msec = millis();

  angular = msg->angular.z; 
  linear = msg->linear.x;

  speed_left =  speed_converter(cinematic_left(angular,linear));
  speed_right = speed_converter(cinematic_right(angular,linear));
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


  set_microros_wifi_transports(keys::wifi_ssid, keys::wifi_pass, keys::agent_ip, keys::agent_port);
  delay(2000);
  Serial.begin(115220);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create subscriber
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
  if (millis() - last_control_msec > watch_dog_msec)
  {
    Serial.println(".");
  }
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));
}
