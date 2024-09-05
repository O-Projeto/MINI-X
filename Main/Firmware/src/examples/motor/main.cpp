    #include <Arduino.h>
    #include <ESP32Servo.h>

    // #include "motor_brushless.h"
    #include "motor_brushless.h"

    // 33 left 
    // 32 tight

    motor_brushless motor_left(33);
    motor_brushless motor_right(32);

    void setup(){
         Serial.begin(9600);
        //  delay(10000);
        motor_right.init();
        motor_left.init();
        delay(2000); 
        
    }

    void loop(){

      for(int i = 0; i < 70; i++){      
          motor_left.write(i);
          motor_right.write(i);
          delay(300);
          Serial.println(i);
      }
      for(int i = 70; 0 < i; i--){      
          motor_left.write(i);
          motor_right.write(i);
          delay(300);
          Serial.println(i);
      }
   }


