
// integrate the enemy and robot localization process and output to the kinematics 
// input -> robot_localization 
// input -> enemy_localizatio 
// output -> linear and angular speed 
#include "localization.h"
#include "enemy_localization.h"
#include "config.h"
#include <Arduino.h>
#include "controller.h"
#include "controller.h" //PID

// =-=-=-=-=-=-=-= PID para 45° =-=-=-=-=-=-=-=
// float KP = 3; //constante correção de erros PID
// float KI = 0.01;
// float KD = 0.0;

// =-=-=-=-=-=-=-= PID para 90° =-=-=-=-=-=-=-=
// float KP = 1.6; //constante correção de erros PID
// float KI = 0.01;
// float KD = 0.0;

// =-=-=-=-=-=-=-= PID para 135° =-=-=-=-=-=-=-=
// float KP = 1.18; //constante correção de erros PID
// float KI = 0.01;
// float KD = 0.0;

// =-=-=-=-=-=-=-= PID para 180° =-=-=-=-=-=-=-=
float KP = 1.088; //constante correção de erros PID
float KI = 0.01;
float KD = 0.0;

struct robot_speed {
     float linear ;
     float angular ; 
};


class MX0
{
private:
    
    localization robot_localization; // Objeto para obter a localização do robô
    // current robot position
    // robot_position robot_pos; // Estrutura para armazenar a posição do robô
    robot_speed emocoes; // Estrutura para armazenar a velocidade linear e angular
    // enemy_localization_cord enemy_info; // Estrutura para armazenar as informações sobre o inimigo
    Enemy_localization enemy; // Objeto para obter a localização do inimigo
    Controller balancer_controller; // Controlador PID para ajustar a orientação
    int angulo_pid;

public:
    robot_position robot_pos; // Estrutura para armazenar a posição do robô
    enemy_localization_cord enemy_info; // Estrutura para armazenar as informações sobre o inimigo
    MX0(/* args */);
    void init();
    robot_speed process(robot_position robot_pos, enemy_localization_cord enemy_info);
    void debug();
   
};

MX0::MX0():balancer_controller(KP, KI, KD){

}

void MX0::init(){
    Wire.begin();
    Wire.setClock(400000);
    enemy.init_sensors(Wire);
    robot_localization.init(Wire);
}

 robot_speed MX0::process(robot_position robot_pos, enemy_localization_cord enemy_info)
 {
    
    // const int long timer_enemy = millis();

    // Serial.print("Robot Pos: ");
    // Serial.println(robot_pos.x);
    // Serial.print("Enemy Info: ");
    // Serial.println(enemy_info.dist);
    // Serial.print("1 inicial: ");
    // Serial.print(emocoes.angular);
    // // Serial.print(" | ");

    angulo_pid = robot_pos.theta + enemy_info.angle;

    if( angulo_pid < PI)
        angulo_pid = angulo_pid +2*PI ;

    if( angulo_pid > PI)
        angulo_pid = angulo_pid - 2*PI ;

    // // Calcula a velocidade angular com base na orientação do robô e a do inimigo
    // emocoes.angular = balancer_controller.output(0, robot_pos.theta);

    const int long timer_pid = millis();
    

    balancer_controller.debug();
    

    
    // Serial.print(" calculo algular: ");
    // Serial.print(emocoes.angular);
    // Serial.print(" = PID*(");
    // Serial.print(robot_pos.theta);
    // Serial.print(" + ");
    // Serial.print(enemy_info.angle);
    // Serial.print(" , ");
   
    

    // Define a velocidade linear com base na distância do inimigo
    emocoes.linear = 0; // Valor padrão pequeno


    return emocoes;    
}

void MX0::debug(){

    // enemy.debug();

   balancer_controller.debug();

    /*
   robot_localization.debug();
   enemy.debug();
   Serial.print(" veloL: ");
   Serial.print(emocoes.linear);
   Serial.print(" | veloA: ");
   Serial.print(emocoes.angular);
   Serial.println();
   */
}



