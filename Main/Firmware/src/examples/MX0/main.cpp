#include <Arduino.h>
#include "MX0.h"
#include "kinematic.h"
#include "LED.h"
#include "controle_juiz.h"

// #include "VL53_sensors.h"

#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 2

#define L 0.09
#define R 0.015

#define CONTADOR_PISCA_LIMITE 50

enum class State
{
    INICIAL,
    COMBATE,
    PARADO,
    PISCA
};
bool pisca_terminou = false;
int contador_pisca = 0;

int juiz_comand;
int ordem_led = 0;
int tempoantigo = 0;
State robot_state = State::INICIAL; // Variavel que guarda qual estado de combate esta

Led led(26, 4, 5, 100);
controle_juiz controle(34);

kinematic robo_moviment(32, 33, L, R);

MX0 razoes;
localization robot_localization; // Objeto para obter a localização do robô
Enemy_localization enemy;        // Objeto para obter a localização do inimigo
robot_position robot_pos;
enemy_localization_cord enemy_info;

robot_speed robo_speed;
// Handle para a fila
QueueHandle_t robotPositionQueue;
QueueHandle_t enemyLocalizationQueue;
void EnemyRobotLocalizationTask(void *pvParameters);
void EnemyRobotLocalizationUpdate();

// VL53_sensors vltest;

void setup()
{
    // Serial.begin(112500);
    Wire.begin();
    Wire.setClock(400000);
    enemy.init_sensors(Wire);

    robot_localization.init(Wire);

    robo_speed.angular = 0;
    robo_speed.linear = 0;
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
        2048,                         // Tamanho da pilha
        NULL,                         // Parâmetro da tarefa
        2,                            // Prioridade da tarefa
        NULL,                         // Handle da tarefa
        0                             // Core
    );
}

void loop()
{

    // ----------------------------------------
    // Maquina de estados
    // ----------------------------------------

    // ----------------------------------------
    // Input
    // ----------------------------------------

    juiz_comand = controle.read();
    // Serial.print("IR: ");
    // Serial.println(juiz_comand);
    // juiz_comand = 1;
    // Serial.print("JC ");
    // Serial.print(juiz_comand);
    // Serial.print(" ");

    // ----------------------------------------
    // Filtro
    // ----------------------------------------
    pisca_terminou = (contador_pisca >= CONTADOR_PISCA_LIMITE);

    // ----------------------------------------
    // Transicoes
    // ----------------------------------------

    switch (robot_state)
    {
    case State::INICIAL:

        if (juiz_comand == BUTTON_1)
            robot_state = State::PISCA;
        if (juiz_comand == BUTTON_2)
            robot_state = State::COMBATE;
        break;

    case State::COMBATE:
        if (juiz_comand == BUTTON_3)
            robot_state = State::PARADO;
        break;

    case State::PARADO:
        break;

    case State::PISCA:

        if (pisca_terminou)
            robot_state = State::INICIAL;
        break;

    default:
        break;
    }

    // ----------------------------------------
    // Transicoes
    // ----------------------------------------

    switch (robot_state)
    {
    case State::INICIAL:

        led.blue();
        contador_pisca = 0;

        break;

    case State::COMBATE:

        led.green();
        EnemyRobotLocalizationUpdate();

        break;

    case State::PARADO:

        led.red();
        robo_speed.angular = 0;
        robo_speed.linear = 0;

        break;

    case State::PISCA:

        led.black();
        contador_pisca++;

        break;

    default:
        break;
    }

    // switch (juiz_comand)
    // {
    // case (BUTTON_1):
    //     if (robot_state == 0)
    //     {
    //         Serial.println("azul");
    //         led.blue();
    //         robot_state = 0; // Modo aguardando
    //         /* code */
    //     }
    //     break;

    // case (1):
    //     if (robot_state == 0 || robot_state == 1)
    //     {
    //         Serial.println("verde");
    //         led.green();
    //         // delay(500);;

    //         EnemyRobotLocalizationUpdate();

    //         /* code */
    //         ordem_led = 1;
    //         robot_state = 1;
    //     }
    //     break;

    // case (2):
    //     if (robot_state == 1 || robot_state == 2)
    //     {
    //         Serial.println("vermelho");
    //         led.red();
    //         robo_speed.angular = 0;
    //         robo_speed.linear = 0;
    //         ordem_led = 0;
    //         robot_state = 2;
    //     }
    //     break;

    // default:
    //     break;
    //     // led.black();
    // }

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
    // Colocar numa task nucleo 0

    razoes.debug();
    // Serial.println("");
    robo_moviment.move(robo_speed.linear, robo_speed.angular);
}
void EnemyRobotLocalizationUpdate()
{

    // Tenta ler da fila sem bloquear
    if (xQueueReceive(enemyLocalizationQueue, &enemy_info, 0) && xQueueReceive(robotPositionQueue, &robot_pos, 0))
    {
        // Serial.println("Lida fila!");
        robo_speed = razoes.process(robot_pos, enemy_info);
        // vTaskDelay(300);
    }
    else
    {
        // Serial.println("Fila NOK!");
        // vTaskDelay(300);
    }
}
void EnemyRobotLocalizationTask(void *pvParameters)
{
    while (1)
    {
        robot_pos = robot_localization.getPosition(); // Obtém a posição atual do robô
        enemy_info = enemy.get_info();
        // enemy_localization_cord enemy_info ;
        // Envia as distâncias para a fila sem bloquear
        xQueueSendFromISR(enemyLocalizationQueue, (void *)&enemy_info, NULL);
        // Envia as distâncias para a fila sem bloquear
        xQueueSendFromISR(robotPositionQueue, (void *)&robot_pos, NULL);
    }
}