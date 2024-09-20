#include "motor_brushless.h"


class kinematic
{
private:
public:

    motor_brushless motor_left;
    motor_brushless motor_right;
    int left_speed ;
    int right_speed ; 
    float linear, angular;
    float L;
    float R ; 
    kinematic(int MOTOR_LEFT_PIN, int MOTOR_RIGHT_PIN,float L, float R );
    void move(float linear,float angular);
    void debug();
    void init();
    ~kinematic();
};

kinematic::kinematic(int MOTOR_LEFT_PIN, int MOTOR_RIGHT_PIN,float L,float R ):motor_left(MOTOR_LEFT_PIN),motor_right(MOTOR_RIGHT_PIN){

    this->L = L;
    this->R = R;
}


kinematic::~kinematic()
{
}

void kinematic::init(){
  motor_left.init();
  motor_right.init();
  //wait for the initialization of the esc 
  delay(2000); 
}

void kinematic::move(float linear,float angular){
  this->linear = linear;
  this->angular = angular;

  right_speed =  (linear + angular*L)/R ; 

  left_speed =  (linear + angular*L)/R ; 

  if(right_speed > 100)
    right_speed = 100;

  if(left_speed > 100)
   left_speed = 100;

  if(right_speed < -100)
    right_speed = -100;

  if(left_speed < -100)
    left_speed = -100;

  motor_left.write(left_speed);
  motor_right.write(-1*right_speed);


}

void kinematic::debug(){
  Serial.print("linear: ");
  Serial.print(linear);
  Serial.print(" angular: ");
  Serial.print(angular);
  Serial.print("left: ");
  Serial.print(left_speed);
  Serial.print("right: ");
  Serial.print(right_speed);


}
