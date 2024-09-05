#include <Arduino.h>                  // Inclui a biblioteca base do Arduino
#include <ESP32Servo.h>               // Inclui a biblioteca para controle de servos no ESP32
#include <ESP32Encoder.h>             // Inclui a biblioteca para leitura de encoders no ESP32
#include "config.h"                   // Inclui o arquivo de configuração com definições e constantes
#include "led_rgb.h"                  // Inclui o arquivo para controle de LEDs RGB
#include "controle_juiz.h"            // Inclui o arquivo para controle do juiz (controle remoto, talvez)
#include "VL53_sensors.h"             // Inclui o arquivo para lidar com sensores VL53
#include "VL53L5CX_sensor.h"          // Inclui o arquivo para lidar com o sensor VL53L5CX
#include "mpu.h"                      // Inclui o arquivo para lidar com o giroscópio MPU
#include "cinematic.h"                // Inclui o arquivo para cálculos cinemáticos
#include "odometry.h"                 // Inclui o arquivo para cálculos de odometria
#include "voltage.h"                  // Inclui o arquivo para monitoramento de tensão
#include "refletancia.h"              // Inclui o arquivo para sensores de refletância
#include "controller.h"               // Inclui o arquivo para o controlador PID

// Definições de constantes e variáveis
#define ROS 1                        // Define uma constante ROS com valor 1
#define N_SENSOR 3                   // Define o número de sensores de distância como 3
#define MIN_PID 0.13                 // Define o valor mínimo para o controle PID
#define MAX_PID 0.4                  // Define o valor máximo para o controle PID

voltage battery_voltage(PIN_BAT);    // Cria um objeto para monitorar a tensão da bateria
float bat_read;                     // Variável para armazenar a leitura da tensão da bateria
bool bat_blink = false;             // Variável para controlar o piscar do LED de bateria baixa

float KP = -0.018;                  // Constante proporcional para o PID
float KI = -0.001;                  // Constante integral para o PID
float KD = -0.000;                  // Constante derivativa para o PID
Controller balancer_controller(KP, KI, KD);  // Cria um objeto controlador PID para balanceamento

unsigned long previousMillis = 0;   // Variável para armazenar o tempo anterior, usada para temporização
unsigned long currentMillis;        // Variável para armazenar o tempo atual, usada para temporização

int const SP_X = 3;                 // Ponto de ajuste X (em cm)
int const SP_Y = 0;                 // Ponto de ajuste Y
int const tolerancia = 0.5;         // Tolerância para o controle PID

bool _imu_connect;                  // Variável para verificar a conexão com o IMU (Unidade de Medida Inercial)
bool _connect = false;             // Variável para verificar conexão geral

float set_point = 0;                // Variável para armazenar o ponto de ajuste do controlador
float kp;                          // Variável para armazenar o ganho proporcional
float pid;                         // Variável para armazenar o valor do controle PID
float error_angular;                // Variável para armazenar o erro angular

int cr_read;                        // Variável para armazenar a leitura do controle do juiz
int comand = -1;                    // Variável para armazenar o comando atual
float* imu_ypr;                     // Ponteiro para armazenar a orientação do IMU (Yaw, Pitch, Roll)

float dx, dy, error_pos_linear;     // Variáveis para armazenar mudanças de posição e erro linear

int led_color = VERMELHO;           // Cor inicial do LED (vermelho)
float angular, linear;              // Variáveis para armazenar a velocidade angular e linear
float angular_ros, linear_ros;      // Variáveis para armazenar a velocidade angular e linear em ROS
float angular_pid, linear_pid;      // Variáveis para armazenar as saídas PID para ângulo e linear
float speed_right, speed_left;      // Variáveis para armazenar a velocidade dos motores direito e esquerdo
bool enable = false;                // Variável para habilitar ou desabilitar o controle

Servo motor_esquerda;               // Objeto para controlar o motor esquerdo (servo)
Servo motor_direita;                // Objeto para controlar o motor direito (servo)
ESP32Encoder encoder_esquerda;      // Objeto para ler o encoder do motor esquerdo
ESP32Encoder encoder_direita;       // Objeto para ler o encoder do motor direito
led_rgb LED;                        // Objeto para controlar LEDs RGB
controle_juiz controle_sony(CR_PIN); // Objeto para controlar o juiz (controle remoto) com o pino CR_PIN

refletancia rft_front(RFT_FRONT_PIN, 2900); // Cria objeto para sensor de refletância frontal
refletancia rft_back(RFT_BACK_PIN, 2900);   // Cria objeto para sensor de refletância traseiro
int border_front;                         // Variável para armazenar a leitura do sensor de refletância frontal
int border_back;                          // Variável para armazenar a leitura do sensor de refletância traseiro

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2, SDIST_3 }; // Define os pinos de shutdown para sensores de distância
VL53_sensors sensores;                      // Cria objeto para lidar com sensores VL53

// Declaração de funções
void debug_serial();                       // Declara função para depuração serial
float recon_enemy_sides(VL53_sensors sensores); // Declara função para reconhecer a posição dos inimigos

void setup()
{
    Serial.begin(115200);               // Inicia a comunicação serial com taxa de 115200 bauds
    Serial.print("Inicio do setup");   // Imprime uma mensagem de início do setup

    motor_direita.attach(pinEsc1);     // Anexa o pino de controle ao motor direito
    motor_esquerda.attach(pinEsc2);    // Anexa o pino de controle ao motor esquerdo

    delay(255);                        // Atraso para garantir a inicialização adequada dos motores

    motor_direita.writeMicroseconds(1500); // Define a posição inicial do motor direito
    motor_esquerda.writeMicroseconds(1500); // Define a posição inicial do motor esquerdo

    ESP32Encoder::useInternalWeakPullResistors = puType::up; // Configura resistores pull-up internos para os encoders
    encoder_esquerda.attachHalfQuad(ENCODER_LEFT_A_PIN, ENCODER_LEFT_B_PIN); // Anexa os pinos dos encoders ao motor esquerdo
    encoder_direita.attachHalfQuad(ENCODER_RIGHT_A_PIN, ENCODER_RIGHT_B_PIN); // Anexa os pinos dos encoders ao motor direito

//    controle_sony.init();               // Inicializa o controle do juiz
//    sensores.sensorsInit();            // Inicializa os sensores VL53

    battery_voltage.calibration(2260, 11.0, 2693, 12.63); // Calibra o monitor de tensão da bateria

    _imu_connect = imu_setup();        // Configura o IMU e armazena se a conexão foi bem-sucedida
    digitalWrite(2, !_imu_connect);   // Configura o pino 2 como saída para indicar a conexão do IMU

//    VL53L5_init();                    // Inicializa o sensor VL53L5

//    LED.init();                       // Inicializa o controle dos LEDs RGB

//    led_color = AZUL;                // Define a cor inicial do LED como azul

//    pinMode(2, OUTPUT);              // Define o pino 2 como saída
}

void loop()
{
    // Obtém dados do IMU
    float* imu_orientation = orientation(); 
    float* imu_angularVelocity = angular_velocity(); 
    float* imu_linaerAcceleration = linear_acceleration();
    float* imu_orientationCovariance = orientation_covariance();
    float* imu_linaerAccelerationCovariance = linear_acceleration_covariance(); 
    float* imu_angularVelocityCovariance = angular_velocity_covariance(); 

    cr_read = controle_sony.read();    // Lê o valor do controle do juiz

    bat_read = battery_voltage.output(); // Obtém a leitura da tensão da bateria

    rft_front.read();                  // Lê o valor do sensor de refletância frontal
    rft_back.read();                   // Lê o valor do sensor de refletância traseiro

    border_front = rft_front.detect_border(); // Detecta a borda com o sensor frontal
    border_back = rft_back.detect_border();   // Detecta a borda com o sensor traseiro

    if (cr_read == -1) {
        led_color = AZUL;             // Se o comando for -1, define a cor do LED como azul
    }

    if (cr_read == TREE || cr_read == TWO) {
        comand = cr_read;             // Se o comando for TREE ou TWO, armazena o comando em 'comand'
    }

    sensores.distanceRead();          // Lê os valores dos sensores de distância
    imu_ypr = imu_get_ypr();          // Obtém a orientação Yaw, Pitch e Roll do IMU

    // float front_position = VL53L5_getFrontPosition(); // (Comentado) Obtém a posição frontal dos sensores VL53L5
    float distance_read = sensores.get_distance(); // Obtém a distância medida pelos sensores VL53

    // Processa o comando do controle do juiz
    switch (comand) {
        case 1:
            // Comando 1
            break;
        case 2:
            // Comando 2
            break;
        // Adicione mais casos conforme necessário
    }

    // Atualiza o LED com base na cor definida
    LED.setColor(led_color);

    // Calcula e aplica a saída dos motores (velocidades)
    float left_speed = ...;  // Calcule a velocidade do motor esquerdo
    float right_speed = ...; // Calcule a velocidade do motor direito
    motor_esquerda.writeMicroseconds(left_speed);
    motor_direita.writeMicroseconds(right_speed);

    delay(100); // Atraso para permitir a leitura e o processamento
}
