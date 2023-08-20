    //PWM control for the motors 
#include <ESP32Servo.h>


//this motor works with values betwen 1000 and 2000 and 1500 is zero, 
// so 1000 -> all speed backward 
// 2000 -> all speed forward 
// 1500 -> stop

class motor_brushless 
{
private:
    Servo esc;
    int MAX_VALUE = 2000; 
    int MIN_VALUE = 1000; 
  
public:
    int pinEsc ;
   
    motor_brushless (int PIN_ESC);
    void init();
    void write(int speed);
};

motor_brushless ::motor_brushless (int PIN_ESC)
{   
    pinEsc = PIN_ESC ;

}
void motor_brushless ::init(){
    //set motor
    esc.attach(pinEsc); 
    // init stoped 
    esc.writeMicroseconds(1500); 
    
};

void motor_brushless ::write(int speed){
    // speed goes from - 100% to 100%
   speed = map(speed,- 100, 100,MIN_VALUE,MAX_VALUE); 
   
   esc.writeMicroseconds(speed); 
   
}