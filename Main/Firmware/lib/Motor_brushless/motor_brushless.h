#include <ESP32Servo.h>

#ifndef ESC // include guard
#define ESC

/// lib developed by Dinossauro Bebado @ O PROJETO 2024
// ----Control a motor brushless with a esc 
// To achieve this control it needs a Eletronic Speed Controler or ESC 
// Its equivalent to a H bridge for a usual brushed dc motor 

// For the ESC works it's need a arm signal so in the init func there is a signal and a delay
// both the signal in miliseconds as the delay can change from esc to esc

// The control it self is easy just init with the esc pin 
// Use the write func to send a value from -100 to 100 % of speed 


//the itself motor works with  signal  betwen 1000(backward) and 2000 (forward) microseconds  and 1500 is zero, 

// pay atention that most escs are unidirection and needs to be reprogramed to works for this lib 

class motor_brushless 
{
private:
    int MAX_VALUE = 2000; //microsends value
    int MIN_VALUE = 1000; //microsends value
  
public:
    Servo esc;
    int pinEsc ;
   
    motor_brushless (int PIN_ESC);
    void init();
    void write(int speed);
};

#endif