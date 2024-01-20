#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <nav_msgs/msg/odometry.h>
#include "config.h"
#include "keys.h"
#include "motor_brushles.h"
#include "cinematic.h"

#define pinEsc2 32
#define pinEsc1 33

float speed_left, speed_right;

Servo motor_esquerda;
Servo motor_direita;

rcl_publisher_t odom_publisher;
nav_msgs__msg__Odometry odom_msg;

rcl_subscription_t subscriber;
geometry_msgs__msg__Twist msg;

rclc_executor_t executor;
rcl_allocator_t allocator;
rclc_support_t support;
rcl_node_t node;
rcl_timer_t timer;

const int watch_dog_msec = 1000;
int last_control_msec = 0;

#define RCCHECK(fn)                  \
  {                                  \
    rcl_ret_t temp_rc = fn;          \
    if ((temp_rc != RCL_RET_OK))     \
    {                                \
      error_loop();                  \
    }                                \
  }
#define RCSOFTCHECK(fn)              \
  {                                  \
    rcl_ret_t temp_rc = fn;          \
    if ((temp_rc != RCL_RET_OK))     \
    {                                \
      error_loop();                  \
    }                                \
  }

void error_loop()
{
  while (1)
  {
    Serial.printf("error_loop\n");
    delay(1);
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    
  }
}


// twist message cb
void subscription_callback(const void *msgin)
{
  const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
  Serial.printf("%lf, %lf\n", msg->linear.x, msg->angular.z);
  speed_left = speed_converter(cinematic_left(msg->linear.x, msg->angular.z));
  speed_right = speed_converter(cinematic_right(msg->linear.x, msg->angular.z));
  last_control_msec = millis();
  motor_esquerda.write(speed_left);
  motor_direita.write(speed_right);
}

void setup()
{
  Serial.begin(115200);
  // this order is important,
  motor_direita.attach(pinEsc1);
  motor_esquerda.attach(pinEsc2);

  // don't remove this delay !!!
  delay(255);

  // init motors
  motor_direita.writeMicroseconds(1500);
  motor_esquerda.writeMicroseconds(1500);

  set_microros_wifi_transports(keys::wifi_ssid, keys::wifi_pass, keys::agent_ip, keys::agent_port);
  delay(2000);
  Serial.begin(115200);

  allocator = rcl_get_default_allocator();

  // create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  rclc_publisher_init_default(
      &odom_publisher,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
      "odom_publisher");

  const unsigned int timer_timeout = 1000;
  rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(timer_timeout),
      timer_callback);

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
      &subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
      "cmd_vel"));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));
  rclc_executor_add_timer(&executor, &timer);
}

void loop()
{
  if (millis() - last_control_msec > watch_dog_msec)
  {
    // Serial.println(".");
  }
  // Initialize Odometry message
    nav_msgs__msg__Odometry odom_msg_init;
    memset(&odom_msg_init, 0, sizeof(nav_msgs__msg__Odometry)); // Initialize the message

    odom_msg_init.pose.pose.position.x += 0.1; // Example: increment X position by 0.1 each time
    odom_msg_init.twist.twist.linear.x = 1.0;  // Example: set linear velocity to 1.0
    Serial.println("pub");
    // Publish the message
    rcl_publish(&odom_publisher, &odom_msg_init, NULL);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));
}
