
// rclc_executor_t executor;
// rcl_allocator_t allocator;
// rclc_support_t support;
// rcl_node_t node;

// rcl_publisher_t pub;
// std_msgs__msg__Float32 pid_p__m;

//   allocator = rcl_get_default_allocator();

//   // create init_options
//   RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

//   // create node
//   RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

// RCCHECK(rclc_publisher_init_default(
//   &pub,
//   &node,
//   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
//   "zero/get/pid/p"));

// RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
// RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msg, &subscription_callback, ON_NEW_DATA));


// // outr funcao
//   pid_p__m.data =  balancer_controller.proportional();
//   RCSOFTCHECK(rcl_publish(&pid_p_t, &pid_p__m, NULL));

