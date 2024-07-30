#include <Arduino.h>
#include "MX0.h"
#include "kinematic.h"
#include "LED.h"
#include "controle_juiz.h"

#define L  0.09 
#define R 0.015

int juiz_comand;
int ordem_led = 0;
int tempoantigo = 0;

Led led(26, 4, 5, 100);
controle_juiz controle(34);

kinematic robo_moviment(32,33,L,R);


MX0 razoes;

robot_speed robo_speed;

void setup(){   
    Serial.begin(112500);
    robo_moviment.init();
    
    razoes.init();
    led.init();

    controle.init();

}

void loop(){
    
    juiz_comand = controle.read();
    Serial.print("JC ");
    Serial.print(juiz_comand);
    Serial.print(" ");

    switch (juiz_comand)
    {
    case (0):
        if (ordem_led == 0){
            led.blue();
        }
            /* code */

        break;

    case (1):
        led.green();
        robo_speed = razoes.process();
        /* code */
        ordem_led = 1;
        break;
    
     case (2):
        led.red();
        robo_speed.angular = 0 ;
        robo_speed.linear = 0 ;
        ordem_led = 0;
        break;
     default:
        led.black();
    }
    
 
    Serial.print(" A: ");
    Serial.print(robo_speed.angular);
    Serial.print(" L: ");
    Serial.print(robo_speed.linear);
    Serial.println("");
    razoes.debug();
    robo_moviment.move(robo_speed.linear,robo_speed.angular);
    
   
}

