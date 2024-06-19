    #include <Arduino.h>
    #include <ESP32Servo.h>

    // #include "motor_brushless.h"
    #include "motor_brushless.h"

    // 33 left 
    //32 tight

    motor_brushless motor_left(33);
    motor_brushless motor_right(32);

    void setup(){
        // Serial.begin(9600);
        //  delay(10000);
        motor_left.init();
        motor_right.init();
        // Serial.print("init init");
       delay(2000); 
    }

    void loop(){


       motor_left.write(70);
       motor_right.write(70);
    }


