#include <Arduino.h>
#include "motor_brushles.h"
#include "mpu.h"
#include "led_rgb.h"
#include "controle_juiz.h"
#include "refletancia.h"

#include "config.h"

int pinPot = 25;

bool imu_connect; 
bool connect = false;

int led_color = VERMELHO;
// init motor -> goes from -100% 2 + 100% 
motor_brushless left_motor(MOTOR_LEFT_PIN);
motor_brushless right_motor(MOTOR_RIGH_PIN);

led_rgb LED ;

controle_juiz controle_sony(CR_PIN);

refletancia rft_front(RFT_FRONT_PIN,700);
refletancia rft_back(RFT_BACK_PIN,700);

void setup() {
    LED.init();
    LED.latch(100,AZUL);

    pinMode(2,OUTPUT);

    left_motor.init();
    right_motor.init();
   
    imu_connect = imu_setup();

    controle_sony.init();

    rft_back.init();
    rft_front.init();


  if(imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }

    //debug potenciometer
    pinMode(pinPot,INPUT);

    Serial.begin(112500);

    delay(500);


    LED.set(VERMELHO);

}

void loop() {


    //inputs 

    int cr_read = controle_sony.read();

    int  detect_border_front = rft_front.read();
    int  detect_border_back = rft_back.read();

    float* imu_ypr = imu_get_ypr(); 

    int val = analogRead(pinPot);
    int speed = map(val, 0, 4096,-100,100); 


   //main_machine state for the fight levels 

    switch (cr_read)
    {
    case ONE :
      //enable robot 
      led_color = AMARELO;
      break;
    
    case TWO :
      // fight 
      led_color = VERDE;
      break;    


    case TREE :
      // stop 
      led_color = AZUL ;
      break;    
    }


    Serial.print(" CR: ");
    Serial.print(cr_read);
    Serial.print(" RFT: ");
    Serial.print(detect_border_back);
    Serial.print(" ");
    Serial.print(detect_border_front);
    Serial.print(" motor: ");
    Serial.print(speed);
    Serial.print("pot: ");
    Serial.print(val); 
    Serial.print(" |IMU  Y :");
    Serial.print(imu_ypr[0]);
    Serial.print(" P : ");
    Serial.print(imu_ypr[1]);
    Serial.print(" R : ");
    Serial.print(imu_ypr[2]);

    Serial.println("");



    LED.set(led_color);
    left_motor.write(30);
    right_motor.write(30);
}