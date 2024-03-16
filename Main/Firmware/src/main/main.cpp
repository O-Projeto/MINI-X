#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESP32Encoder.h>
#include "config.h" 
#include "led_rgb.h"
#include "controle_juiz.h"
#include "VL53_sensors.h"
// #include "VL53L5CX_sensor.h"
#include "mpu.h" //giroscopio 
#include "cinematic.h"
#include "odometry.h"

#include "voltage.h"
voltage battery_voltage(PIN_BAT);
float bat_read; 


#include "controller.h" //PID
double KP = 0.1 ; //constante correção de erros PID

Controller balancer_controller(KP,0,0); 


unsigned long previousMillis = 0; //unsigned significa que não guarda numero negativo
unsigned long currentMillis ;  

int const SP_X = 3; //[cm ]
int const SP_Y = 0;
int const tolerancia = 0.5;


bool _imu_connect; 
bool _connect = false;

float set_point = 0;


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

#include "refletancia.h"
refletancia rft_front(RFT_FRONT_PIN, 2900);
int border_front ;

refletancia rft_back(RFT_BACK_PIN,2900);
int border_back;






#define N_SENSOR 3

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2,SDIST_3 } ; 

VL53_sensors sensores;

void debug_serial();





float recon_enemy_sides(VL53_sensors sensores);





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
   

    controle_sony.init();

    sensores.sensorsInit();

    battery_voltage.calibration(2260,11.0,2693,12.63);

    _imu_connect = imu_setup();

    digitalWrite(2, !_imu_connect); 

    // VL53L5_init();
    
    //init led 
    LED.init();

    led_color = AZUL ;

    pinMode(2,OUTPUT);



}

void loop()
   {
  
      cr_read = controle_sony.read();

      bat_read =  battery_voltage.output(); 

      rft_front.read();
      rft_back.read();

      border_front = rft_front.detect_border();
      border_back = rft_back.detect_border();
       
      if(cr_read==-1){
            led_color =AZUL;
          }

        if( cr_read==TREE || cr_read==TWO )
          comand = cr_read ; 

        sensores.distanceRead();
        imu_ypr = imu_get_ypr(); 
        set_point = recon_enemy_sides(sensores);
        // if(border_front){
        //   set_point = PI ; 
        // }
        // if(border_back){
        //   set_point = 0 ;
        // }

        pid = balancer_controller.output(imu_ypr[0],set_point);
        error_angular = balancer_controller.error ; 
        angular = pid ; 

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
                // fight 


              
             
                enable = true ;
                led_color = VERDE;
                linear = 0;

                // if(border_front){
                //   linear = linear*-1;
                // }
                // if(border_back){
                //    linear = linear*-1;
                // }

                
                
                
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


    debug_serial();
    // debug_blu();
    odom(encoder_esquerda.getCount(), -1*encoder_direita.getCount(),imu_ypr[0]);
     


  
    if(set_point == 0){
      // Serial.print("frente");
      LED.set_led_number(led_color,0);
      LED.set_led_number(VERMELHO,1);
      LED.set_led_number(VERMELHO,2);
      LED.set_led_number(led_color,3);
    }
    if(set_point > 1.57 && set_point < 3){
      // Serial.print("direita");
      LED.set_led_number(VERMELHO,0);
      LED.set_led_number(VERMELHO,1);
      LED.set_led_number(led_color,2);
      LED.set_led_number(led_color,3);
    }
    if(set_point < -1.57 && set_point > -3){
      //  Serial.print("esquerda");
      LED.set_led_number(led_color,0);
      LED.set_led_number(led_color,1);
      LED.set_led_number(VERMELHO,2);
      LED.set_led_number(VERMELHO,3);
    }
    if(set_point > -3 && set_point >3){
      //  Serial.print("traz");
      LED.set_led_number(VERMELHO,0);
      LED.set_led_number(led_color,1);
      LED.set_led_number(led_color,2);
      LED.set_led_number(VERMELHO,3);
    }

    motor_esquerda.write(speed_left); 
    motor_direita.write(speed_right); 
}


void debug_serial(){

  // Serial.print(" CR: ");
  // Serial.print(cr_read);
  Serial.print(" CMD:");
  Serial.print(comand);

  Serial.print("Bat ");
  Serial.print(bat_read);

  Serial.print(" |FRONT: ");
  Serial.print(border_front);

  Serial.print(" |BACK: ");
  Serial.print(border_back);

  Serial.print(" |VL :");
  for (uint8_t i = 0; i < N_SENSOR; i++){

      Serial.print(" ");
      Serial.print(String(i));
      Serial.print(" ");
      Serial.print(sensores.dist[i]);

  }



      // Serial.print(" |IMU  Y :");
      // Serial.print(imu_ypr[0]);
      // Serial.print(" P : ");
      // Serial.print(imu_ypr[1]);
      // Serial.print(" R : ");
      // Serial.print(imu_ypr[2]);



      Serial.print( " |SP: ");
      Serial.print(set_point);
      Serial.print(" |PID-e: ");
      Serial.print(error_angular);
      Serial.print(" P: ");
      // Serial.print(kp);

      Serial.print(" |Speed:linear: ");
      Serial.print(linear);
      Serial.print(" angular: ");
      Serial.print(angular);
      Serial.print(" L: ");
      Serial.print(speed_left);
      Serial.print(" R: ");
      Serial.print(speed_right);

      // Serial.print(" X: ");
      // Serial.print(x);
      // Serial.print(" Y: ");
      // Serial.print(y);
      Serial.print(" YAW: ");
      Serial.print(th);



      // Serial.print(" |ENC R: ");
      // Serial.print(-1*encoder_direita.getCount());
      // Serial.print(" | L: ");
      // Serial.print(encoder_esquerda.getCount());

      Serial.println("");





}


float recon_enemy_sides(VL53_sensors sensores){


  float enemy_positon[3] = {PI/2,PI,-PI/2};
  
  for (uint8_t i = 0; i < N_SENSOR; i++){

     
      if(sensores.dist[i] < 50){
        return enemy_positon[i] ;
      } 

  }
    return 0.0 ;



}