#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <geometry_msgs/msg/twist.h>
#include <geometry_msgs/msg/pose_stamped.h>

#include <micro_ros_utilities/type_utilities.h>
#include <micro_ros_utilities/string_utilities.h>

#include <rmw_microros/rmw_microros.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32.h>

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

bool manual = false;

#include "controller.h" //PID
float KP = -0.000 ; //constante correção de erros PID
float KI = 0.0;
float KD = -0.0000;

Controller balancer_controller(KP,KI,KD); 

#define  DEBUG  



bool _imu_connect; 
bool _connect = false;
float* imu_ypr ;

float linear_manual = 0 ;
float angular_manual = 0 ;

float linear_auto = 0 ;
float angular_auto = 0 ;


float speed_left = 0 ;
float speed_right = 0 ;
float pid = 0 ;
float set_point = 0 ;


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

rcl_publisher_t pose_t; 
geometry_msgs__msg__PoseStamped pose_m;


//pid

//p
rcl_publisher_t pid_p_t;
std_msgs__msg__Float32 pid_p__m;

// i 
rcl_publisher_t pid_i_t;
std_msgs__msg__Float32 pid_i_m;

//d 
rcl_publisher_t pid_d_t;
std_msgs__msg__Float32 pid_d_m;

// output
rcl_publisher_t pid_out_t;
std_msgs__msg__Float32 pid_out_m;

//setpoint 
rcl_publisher_t sp_t;
std_msgs__msg__Float32 sp_m;

//feedback
rcl_publisher_t feedback_t;
std_msgs__msg__Float32 feedback_m;


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
    #ifdef  DEBUG
     Serial.printf("error_loop\n");
    #endif  //DEBUG
    delay(1);
  }
}

// twist message cb
void subscription_callback(const void *msgin)
{
  const geometry_msgs__msg__Twist *msg = (const geometry_msgs__msg__Twist *)msgin;
  
  #ifdef  DEBUG
    Serial.printf("%f, %f\n", msg->linear.x, msg->angular.z);
  #endif  //DEBUG
  // last_control_msec = millis();

  angular_manual = msg->angular.z; 
  linear_manual = msg->linear.x;

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

  #ifdef  DEBUG
    Serial.begin(115220);
  #endif  //DEBUG

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
 
  //------------pid 

  RCCHECK(rclc_publisher_init_default(
  &pid_p_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/p"));

  RCCHECK(rclc_publisher_init_default(
  &pid_i_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/i"));

  RCCHECK(rclc_publisher_init_default(
  &pid_d_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/d"));

  RCCHECK(rclc_publisher_init_default(
  &pid_out_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/out"));

  RCCHECK(rclc_publisher_init_default(
  &sp_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/setpoint"));

  RCCHECK(rclc_publisher_init_default(
  &feedback_t,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "zero/get/pid/feedback"));
  


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

  //get time 
  struct timespec tv = {0};
  clock_gettime(0, &tv);

  imu_ypr = imu_get_ypr(); 
  odom(encoder_esquerda.getCount(), -1*encoder_direita.getCount(),imu_ypr[0]);

  double theta_half = th / 2.0;
  double sin_theta_half = sin(theta_half);
  double cos_theta_half = cos(theta_half);

  

  pose_m.pose.orientation.x = 0.0;
  pose_m.pose.orientation.y = 0.0;
  pose_m.pose.orientation.z = cos_theta_half;
  pose_m.pose.orientation.w = sin_theta_half;
    
  pose_m.pose.position.x = x ; 
  pose_m.pose.position.y = y; 



  pose_m.header.stamp.sec = tv.tv_sec;
  pose_m.header.stamp.nanosec = tv.tv_nsec;

  pose_m.header.frame_id.size = 20;
  pose_m.header.frame_id.data = "pose_frame";


  pid = balancer_controller.output(set_point,imu_ypr[0]);

  // add saturation 

  if(pid>0.4){
    pid = 0.4;
  }
  if(pid <-0.4){
    pid = -0.4;
  }
  
  //tolerancia
  if(balancer_controller.error < 0.2 && balancer_controller.error > -0.2 ){
      pid = 0 ;
  }


  //vel minima pro robo andar 
  if(pid > 0 && pid < 0.14){
    pid = 0.14 ; 
  }

  if(pid < 0 && pid > -0.14){
    pid = -0.14 ; 
  }

  angular_auto = pid ; 

  if(manual){

    speed_left =  speed_converter(-cinematic_left(angular_manual,linear_manual));
    speed_right = speed_converter(cinematic_right(angular_manual,linear_manual));

  }else{
    speed_left =  speed_converter(-cinematic_left(angular_auto,linear_auto));
    speed_right = speed_converter(cinematic_right(angular_auto,linear_auto));
  }

  // Serial.println(speed_left);
  motor_esquerda.write(speed_left); 
  motor_direita.write(speed_right); 

  pid_p__m.data =  balancer_controller.proportional();
  RCSOFTCHECK(rcl_publish(&pid_p_t, &pid_p__m, NULL));

  pid_i_m.data =  balancer_controller.integrative();
  RCSOFTCHECK(rcl_publish(&pid_i_t, &pid_i_m, NULL));

  // pid_d_m.data =  balancer_controller.derivative() + balancer_controller.proportional();
   pid_d_m.data = balancer_controller.derivative();
  RCSOFTCHECK(rcl_publish(&pid_d_t, &pid_d_m, NULL));

  pid_out_m.data =  angular_auto;
  RCSOFTCHECK(rcl_publish(&pid_out_t, &pid_out_m, NULL));

  sp_m.data =  balancer_controller.setpoint_;
  RCSOFTCHECK(rcl_publish(&sp_t, &sp_m, NULL));

  feedback_m.data = imu_ypr[0] ;
  RCSOFTCHECK(rcl_publish(&feedback_t, &feedback_m, NULL));


  RCSOFTCHECK(rcl_publish(&pose_t, &pose_m, NULL));
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));

  #ifdef  DEBUG
    balancer_controller.debug();
    Serial.print(" angular_auto ");
    Serial.print(angular_auto);
    Serial.println("");
  #endif  //DEBUG
}


