#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h" 
#include "led_rgb.h"
#include "controle_juiz.h"
#include "VL53_sensors.h"
#include "mpu.h"



bool _imu_connect; 
bool _connect = false;


float kp,ki,kd, pid_; 


int cr_read;
float* imu_ypr ;

int const MIN_VALUE = 1000; 
int const MAX_VALUE = 2000; 

int led_color = VERMELHO;

float angular, linear ; 
float speed_right, speed_left;
bool enable = false;

Servo motor_esquerda;
Servo motor_direita ;

led_rgb LED ; 

controle_juiz controle_sony(CR_PIN) ; 

#define N_SENSOR 3

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2,SDIST_3 } ; 

VL53_sensors sensores;


#define pinEsc2 32
#define pinEsc1 33

int speed_converter(int speed){

    speed = map(speed,- 100, 100,MIN_VALUE,MAX_VALUE); 

    if(speed < 150 and speed > 1450){
        speed = 1500; 
    }
    return speed ; 
}

float cinematic_right(int linear, int angular){

  float right_speed = (linear + angular*L)/R ; 

  if(right_speed > 100)
    right_speed = 100;

  if(right_speed < -100)
    right_speed = -100;

  return right_speed; 

}

float cinematic_left(float linear, float  angular){

  float left_speed = (linear - angular*L)/R ;

  if(left_speed > 100)
    left_speed = 100;

  if(left_speed < -100)
    left_speed = -100;

  //it reversed because of the mecaical assembly 
  return left_speed; 

}


void debug(){

  Serial.print(" CR: ");
  Serial.print(cr_read);

  Serial.print(" |VL :");
  for (uint8_t i = 0; i < N_SENSOR; i++){

      Serial.print(" ");
      Serial.print(String(i));
      Serial.print(" ");
      Serial.print(sensores.dist[i]);

  }



      Serial.print(" |IMU  Y :");
      Serial.print(imu_ypr[0]);
      Serial.print(" P : ");
      Serial.print(imu_ypr[1]);
      Serial.print(" R : ");
      Serial.print(imu_ypr[2]);


      Serial.print(" |Speed:linear: ");
      Serial.print(linear);
      Serial.print(" angular: ");
      Serial.print(angular);
      Serial.print(" L: ");
      Serial.print(speed_left);
      Serial.print(" R: ");
      Serial.print(speed_right);

      Serial.println("");



}

void setup()
{


    //this order is important, 
    motor_direita.attach(pinEsc1); 
    motor_esquerda.attach(pinEsc2); 

    delay(255);

    motor_direita.writeMicroseconds(1500); 
    motor_esquerda.writeMicroseconds(1500); 

    Serial.begin(115200);

    controle_sony.init();

    sensores.sensorsInit();

    _imu_connect = imu_setup();

    digitalWrite(2, !_imu_connect); 

    // init motors 

    //init led 
    LED.init();


    led_color = AZUL ;

    pinMode(2,OUTPUT);



}

void loop()
{ 

    cr_read = controle_sony.read();
    sensores.distanceRead();
    imu_ypr = imu_get_ypr(); 


    switch (cr_read)
        {
          case ONE :
            //enable robot 
            enable = false; 
            led_color = AMARELO; 
            break;
          
          case TWO :
            // fight  
            enable = true ;
            led_color = VERDE;



            linear = 10;
            angular = 0;
            break;    


          case TREE :
            enable = false ;
            // stop 
            led_color = AZUL ;
            linear = 0;
            angular = 0;
            break;   

          default:
            led_color = AZUL ;
        }

      if(enable){
        speed_left =  speed_converter(cinematic_left(angular,linear));
        speed_right = speed_converter(cinematic_right(angular,linear));
      }else{
        speed_left =  1500;
        speed_right = 1500;
      }


      LED.set(led_color);

     
      motor_esquerda.writeMicroseconds(speed_left); 
      motor_direita.writeMicroseconds(speed_right); 
  
      debug(); 



}

