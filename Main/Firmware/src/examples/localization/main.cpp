#include "localization.h"
#include <Arduino.h>
#include "kinematic.h"

// Declare the localization object globally or within the appropriate scope
localization robot_pos;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    robot_pos.init();

    // Initialize any other necessary components here
}

void loop() {
    // Retrieve the robot's position using the localization object
    robot_position robot = robot_pos.getPosition();

    // Use the robot's position for whatever purpose you need
    // For example, print the position for debugging
    Serial.print("X: ");
    Serial.print(robot.x);
    Serial.print(" Y: ");
    Serial.print(robot.y);
    Serial.print(" Theta: ");
    Serial.println(robot.theta);

    // Add a delay to avoid flooding the serial output
    delay(1000);
}