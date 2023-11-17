#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESP32Encoder.h>
#include "config.h" 
#include "led_rgb.h"
#include "controle_juiz.h"
// #include "VL53_sensors.h"
#include "mpu.h"
#include "cinematic.h"
#include "odometry.h"


#include "controller.h"
Controller balancer_controller(0.5,0,0); 

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;




unsigned long previousMillis = 0; 
unsigned long currentMillis ; 

int const SP_X = 3; //[cm ]
int const SP_Y = 0;
int const tolerancia = 0.5;


bool mode_auto = true;
bool mode_manual = false
;
// bool mode = mode_manual; 
bool mode = mode_auto; 



bool _imu_connect; 
bool _connect = false;

float set_point = 0;

float KP = 0.5 ;

float kp; 
float pid ; 
float error_angular;

int cr_read;
int comand = -1; 
float* imu_ypr ;

float dx,dy,error_pos_linear ; 



int led_color = VERMELHO;

float angular, linear ; 
float angular_pid, linear_pid;
float speed_right, speed_left;
bool enable = false;

Servo motor_esquerda;
Servo motor_direita ;

ESP32Encoder encoder_esquerda;
ESP32Encoder encoder_direita;

led_rgb LED ; 

controle_juiz controle_sony(CR_PIN) ; 

// #define N_SENSOR 3

// int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2,SDIST_3 } ; 

// VL53_sensors sensores;


#define pinEsc2 32
#define pinEsc1 33





void setup()
{


    //this order is important, 
    motor_direita.attach(pinEsc1); 
    motor_esquerda.attach(pinEsc2); 

    //dont remove this delay !!!
    delay(255);

    // init motors 
    motor_direita.writeMicroseconds(1500); 
    motor_esquerda.writeMicroseconds(1500); 

    ESP32Encoder::useInternalWeakPullResistors=UP;
    encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN);
    encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN,ENCODER_RIGHT_B_PIN);
    

    Serial.begin(115200);
    SerialBT.begin("MINI_X"); 

    controle_sony.init();

    // sensores.sensorsInit();

    _imu_connect = imu_setup();

    digitalWrite(2, !_imu_connect); 


    //init led 
    LED.init();

    led_color = AZUL ;

    pinMode(2,OUTPUT);



}

void loop()
   {
    if(mode==mode_auto){

      cr_read = controle_sony.read();
      if(cr_read==-1){
            led_color =AZUL;
          }

        if( cr_read==TREE || cr_read==TWO )
          comand = cr_read ; 

        // sensores.distanceRead();
        imu_ypr = imu_get_ypr(); 

        // cr_read = TWO;
        // Serial.println(cr_read);


      pid = balancer_controller.output(imu_ypr[0],set_point);

      //calculate position error with hipotenuse 
      dx = SP_X - x ;
      dy = SP_Y - y ; 

      error_pos_linear = sqrt( dx*dx + dy*dy);


      // case to select the mode 
      switch (comand)
            {
              case ONE :
                //enable robot 
                // enable = false; 
                led_color = AMARELO; 
                break;
              
              case TWO :
                
              
              
              if(error_pos_linear < tolerancia){
                // stop 
                linear = 0;
                angular = 0;


              }else{
                // linear = linear_pid;
                linear = 4 ;
                angular_pid = pid ;
                angular = angular_pid;
              }

                // fight  
                enable = true ;
                led_color = VERDE;
              
                
                break;    



              case TREE :
              
                enable = false ;
                // stop 
                led_color = VERMELHO ;
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

          if(cr_read==ONE){
            led_color = AMARELO;
          }


 }



    debug_serial();
    debug_blu();
    odom(encoder_esquerda.getCount(), -1*encoder_direita.getCount(),imu_ypr[0]);
     


    LED.set(led_color);
    motor_esquerda.write(speed_left); 
    motor_direita.write(speed_right); 
}


void debug_blu(){

  // SerialBT.print(" CR: ");
  // SerialBT.print(cr_read);
  // SerialBT.print(" CMD:");
  // SerialBT.print(comand);

  // SerialBT.print(" |VL :");
  // for (uint8_t i = 0; i < N_SENSOR; i++){

  //     SerialBT.print(" ");
  //     SerialBT.print(String(i));
  //     SerialBT.print(" ");
  //     SerialBT.print(sensores.dist[i]);

  // }



      // SerialBT.print(" |IMU  Y :");
      // SerialBT.print(imu_ypr[0]);
      // SerialBT.print(" P : ");
      // SerialBT.print(imu_ypr[1]);
      // SerialBT.print(" R : ");
      // SerialBT.print(imu_ypr[2]);


     
      // SerialBT.print(" L: ");
      // SerialBT.print(speed_left);
      // SerialBT.print(" R: ");
      // SerialBT.print(speed_right);


      // SerialBT.print(" |ENC R: ");
      // SerialBT.print(-1*encoder_direita.getCount());
      // SerialBT.print(" | L: ");
      // SerialBT.print(encoder_esquerda.getCount());

      SerialBT.print("|ps: ");
      SerialBT.print(error_pos_linear);
    
      SerialBT.print(" X: ");
      SerialBT.print(x);
      SerialBT.print(" Y: ");
      SerialBT.print(y);
      SerialBT.print(" th: ");
      SerialBT.print(th);

      // SerialBT.print(" |SP: ");
      // SerialBT.print(balancer_controller.setpoint_);
      // SerialBT.print(" |CV: ");
      // SerialBT.print(balancer_controller.current_value_);

      SerialBT.print(" ||e: ");
      SerialBT.print(balancer_controller.error);
      SerialBT.print(" |P: ");
      SerialBT.print(balancer_controller.proportional());
      SerialBT.print(" |I: ");
      SerialBT.print(balancer_controller.integrative());
      SerialBT.print("|D: ");
      SerialBT.print(balancer_controller.derivative());

      SerialBT.print(" |OV: ");
      SerialBT.print(balancer_controller.output_value);
      // SerialBT.print(" linear: ");
      // SerialBT.print(linear);
      // SerialBT.print(" angular: ");
      // SerialBT.print(angular);
      SerialBT.println("");





      SerialBT.println("");



}


void debug_serial(){

  Serial.print(" CR: ");
  Serial.print(cr_read);
  Serial.print(" CMD:");
  Serial.print(comand);

  // Serial.print(" |VL :");
  // for (uint8_t i = 0; i < N_SENSOR; i++){

  //     Serial.print(" ");
  //     Serial.print(String(i));
  //     Serial.print(" ");
  //     Serial.print(sensores.dist[i]);

  // }



      // Serial.print(" |IMU  Y :");
      // Serial.print(imu_ypr[0]);
      // Serial.print(" P : ");
      // Serial.print(imu_ypr[1]);
      // Serial.print(" R : ");
      // Serial.print(imu_ypr[2]);


      // Serial.print(" |Speed:linear: ");
      // Serial.print(linear);
      // Serial.print(" angular: ");
      // Serial.print(angular);
      // Serial.print(" L: ");
      // Serial.print(speed_left);
      // Serial.print(" R: ");
      // Serial.print(speed_right);

      // Serial.print(" |PID-e: ");
      // Serial.print(error_angular);
      // Serial.print(" P: ");
      // Serial.print(kp);

      Serial.print(" X: ");
      Serial.print(x);
      Serial.print(" Y: ");
      Serial.print(y);
      Serial.print(" YAW: ");
      Serial.print(th);



      Serial.print(" |ENC R: ");
      Serial.print(-1*encoder_direita.getCount());
      Serial.print(" | L: ");
      Serial.print(encoder_esquerda.getCount());

      Serial.println("");





}