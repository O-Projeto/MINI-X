#include "motor_brushless.h"


motor_brushless ::motor_brushless (int PIN_ESC)
{   
    pinEsc = PIN_ESC ;

   
}

void motor_brushless::init(){


   esc.attach(pinEsc); 
   esc.writeMicroseconds(1500); 
   delay(255);

}


void motor_brushless ::write(int speed){
    // speed goes from - 100% to 100%
   speed = map(speed,- 100, 100,MIN_VALUE,MAX_VALUE); 
   esc.writeMicroseconds(speed);
   
}