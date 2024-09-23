#include <Arduino.h>
#include "MX0.h"
#include "kinematic.h"
#include "LED.h"
#include "controle_juiz.h"

// #include "VL53_sensors.h"

#define L  0.09 
#define R 0.015

int juiz_comand;
int ordem_led = 0;
int tempoantigo = 0;

Led led(26, 4, 5, 100);
controle_juiz controle(34);

kinematic robo_moviment(32,33,L,R);


MX0 razoes;
localization robot_localization; // Objeto para obter a localização do robô
Enemy_localization enemy; // Objeto para obter a localização do inimigo

robot_speed robo_speed;
// Handle para a fila
QueueHandle_t robotPositionQueue;
QueueHandle_t enemyLocalizationQueue;
void EnemyRobotLocalizationTask(void *pvParameters);
void EnemyRobotLocalizationUpdate();

// VL53_sensors vltest;

void setup(){   
    Serial.begin(112500);
    Wire.begin();
    Wire.setClock(400000);
    // enemy.init_sensors(Wire);
    robot_localization.init(Wire);

    robo_speed.angular = 0 ;
    robo_speed.linear = 0 ;
    robo_moviment.init();
    
    // razoes.init();
    led.init();

    controle.init();
    Serial.println();
    Serial.println("Eu estou funcionando, to esperando você apertar o botão do controle XD");

    // Cria a fila
    robotPositionQueue = xQueueCreate(10, sizeof(robot_position));
    enemyLocalizationQueue = xQueueCreate(10, sizeof(enemy_localization_cord));
    // Cria a tarefa no Core 1 para ler as distâncias dos sensores
    xTaskCreatePinnedToCore(
        EnemyRobotLocalizationTask,   // Função da tarefa
        "EnemyRobotLocalizationTask", // Nome da tarefa
        2048,              // Tamanho da pilha
        NULL,              // Parâmetro da tarefa
        2,                 // Prioridade da tarefa
        NULL,              // Handle da tarefa
        0                  // Core
    );
    
}

void loop(){
    
    juiz_comand = controle.read();
    juiz_comand = 1;
    // Serial.print("JC ");
    // Serial.print(juiz_comand);
    // Serial.print(" ");

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
        // delay(500);;
    
        
        EnemyRobotLocalizationUpdate();

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
    
    /*
    Serial.print(" A: ");
    Serial.print(robo_speed.angular);

    Serial.print(" L: ");
    Serial.print(robo_speed.linear);
    */
    // razoes.debug();
    // robo_moviment.debug();

    // razoes.debug();
    // Serial.println("");
    //Colocar numa task nucleo 0
    
    

    razoes.debug();
    // Serial.println("");
    robo_moviment.move(robo_speed.linear,robo_speed.angular);
}
void EnemyRobotLocalizationUpdate()
{
    robot_position robot_pos;
    enemy_localization_cord enemy_info;
    // Tenta ler da fila sem bloquear
    if (xQueueReceive(enemyLocalizationQueue, &enemy_info, 0) && xQueueReceive(robotPositionQueue, &robot_pos, 0))
    {
        //Serial.println("Lida fila!");
        robo_speed = razoes.process(robot_pos, enemy_info);
        //vTaskDelay(300);
    }
    else
    {
        //Serial.println("Fila NOK!");
        //vTaskDelay(300);
    }
}
void EnemyRobotLocalizationTask(void *pvParameters)
{
    while(1)
    {
        robot_position robot_pos = robot_localization.getPosition(); // Obtém a posição atual do robô
        // enemy_localization_cord enemy_info = enemy.get_info();
        enemy_localization_cord enemy_info ;
        // Envia as distâncias para a fila sem bloquear
        xQueueSendFromISR(enemyLocalizationQueue, ( void * ) &enemy_info, NULL);
        // Envia as distâncias para a fila sem bloquear
        xQueueSendFromISR(robotPositionQueue, ( void * ) &robot_pos, NULL);
    }

}