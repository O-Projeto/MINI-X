#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESP32Encoder.h>
#include "config.h" 
#include "led_rgb.h"
#include "controle_juiz.h"
#include "VL53_sensors.h"
#include "mpu.h"
#include "cinematic.h"
#include "odometry.h"
// #include "ps4.h"




unsigned long previousMillis = 0; 
unsigned long currentMillis ; 
int change_dir_time = 300; 
int dir = 1;


bool mode_auto = true;
bool mode_manual = false
;
// bool mode = mode_manual; 
bool mode = mode_auto; 

bool connected_ps4 = false;

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



int led_color = VERMELHO;

float angular, linear ; 
float speed_right, speed_left;
bool enable = false;

Servo motor_esquerda;
Servo motor_direita ;

ESP32Encoder encoder_esquerda;
ESP32Encoder encoder_direita;

led_rgb LED ; 

controle_juiz controle_sony(CR_PIN) ; 

#define N_SENSOR 3

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2,SDIST_3 } ; 

VL53_sensors sensores;


#define pinEsc2 32
#define pinEsc1 33






void debug(){

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



      Serial.print(" |IMU  Y :");
      Serial.print(imu_ypr[0]);
      Serial.print(" P : ");
      Serial.print(imu_ypr[1]);
      Serial.print(" R : ");
      Serial.print(imu_ypr[2]);


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


      Serial.print(" |ENC R: ");
      Serial.print(-1*encoder_direita.getCount());
      Serial.print(" | L: ");
      Serial.print(encoder_esquerda.getCount());

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

    ESP32Encoder::useInternalWeakPullResistors=UP;
    encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN);
    encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN,ENCODER_RIGHT_B_PIN);
    

    Serial.begin(115200);

    controle_sony.init();

    sensores.sensorsInit();

    _imu_connect = imu_setup();

    digitalWrite(2, !_imu_connect); 

    // init motors 

    //init led 
    LED.init();

    // PS4setup();


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

        sensores.distanceRead();
        imu_ypr = imu_get_ypr(); 

        // cr_read = TWO;
        // Serial.println(cr_read);


      error_angular = set_point - imu_ypr[0] ; 
      kp = error_angular*KP; 

      pid = kp ;



        switch (comand)
            {
              case ONE :
                //enable robot 
                // enable = false; 
                led_color = AMARELO; 
                break;
              
              case TWO :
              currentMillis = millis () ; 
              if ( currentMillis - previousMillis >= change_dir_time ) {

                previousMillis = currentMillis;
                dir = dir*-1; 
              
              }
                // fight  
                enable = true ;
                led_color = VERDE;
              
                linear = 8*dir;
                angular = dir*pid;
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
// else{
//     // connected_ps4 = ps4Connected();

//     // if(connected_ps4){
//     // // manual mode
//     //   led_color = ROXO;
//     //   linear = ps4Linear();
//     //   angular = ps4Angular();
//     //   speed_left =  speed_converter(cinematic_left(angular,linear));
//     //   speed_right = speed_converter(cinematic_right(angular,linear));
//     // }

// }

    LED.set(led_color);
    motor_esquerda.writeMicroseconds(speed_left); 
    motor_direita.writeMicroseconds(speed_right); 


    debug();
    odom(encoder_esquerda.getCount(), -1*encoder_direita.getCount(),imu_ypr[0]);
     


}

