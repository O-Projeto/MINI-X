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
    Servo esc_ ;
    // int MAX_VALUE = 2000; 
    // int MIN_VALUE = 1000; 
  
public:
    int pinEsc ;
    int pinEsc_;
   
    motor_brushless (int PIN_ESC, int PIN_ESC_);
    void init();
    void write(int speed);
};

motor_brushless ::motor_brushless (int PIN_ESC,int  PIN_ESC_)
{   
    pinEsc = PIN_ESC ;
    pinEsc_ = PIN_ESC_; 

}
void motor_brushless ::init(){
    //set motor
    // pinMode(pinEsc,OUTPUT);
    esc.attach(pinEsc);
    esc_.attach(pinEsc_);
    delay(255);
    // init stoped 
    esc.writeMicroseconds(1500); 
    esc_.writeMicroseconds(1500); 
    
};

void motor_brushless ::write(int speed){
    // speed goes from - 100% to 100%
   speed = map(speed,- 100, 100,MIN_VALUE,MAX_VALUE); 
   
   esc.writeMicroseconds(speed);
   esc_.writeMicroseconds(speed);
   
}