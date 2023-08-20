//Bibliotecas externas
#include <Wire.h>       //Auxiliar dos sensores
#include <VL53L0X.h>    // Usar a biblioteca da POLULU!

// MAPA PARA CONEXÕES ELETRÔNICAS

// ADICIONAR UM RESISTOR DE 4.7K ENTRE O PINO SCL E O 3.3V APENAS NO PRIMEIRO SENSOR
// ADICIONAR UM RESISTOR DE 4.7K ENTRE O PINO SDA E O 3.3V APENAS NO PRIMEIRO SENSOR
  

// SCL, SDA, as mesmas portas para todos os sensores
#define SCL_SDIST 22
#define SDA_SDIST 21

// XSHUT, uma porta por sensor 
#define SDIST_1 25
#define SDIST_2 23
#define SDIST_3 13


//Cria os objetos para cada sensor
VL53L0X sensor1;  //Sensor da esquerda
VL53L0X sensor2;  //Sensor da frente
VL53L0X sensor3;  //Sensor da direita

//Variáveis que vão receber os valores lidos dos pinos dos sensores
int dist1;  //Valor lido pelo sensor 1
int dist2;  //Valor lido pelo sensor 2
int dist3;  //Valor lido pelo sensor 3

void sensorsInit() {

    //Iniciando o endereçamento dos sensores
    Wire.begin();

    pinMode(SDIST_1, OUTPUT);
    pinMode(SDIST_2, OUTPUT);
    pinMode(SDIST_3, OUTPUT);

    digitalWrite(SDIST_1, LOW);
    digitalWrite(SDIST_2, LOW);
    digitalWrite(SDIST_3, LOW);

    pinMode(SDIST_1, INPUT);
    sensor1.init(true);
    sensor1.setAddress((uint8_t)0x21); //endereço do sensor 1

    pinMode(SDIST_2, INPUT);
    sensor2.init(true);
    sensor2.setAddress((uint8_t)0x22); //endereço do sensor 2

    pinMode(SDIST_3, INPUT);
    sensor3.init(true);
    sensor3.setAddress((uint8_t)0x23); //endereço do sensor 3

    sensor1.setTimeout(40);
    sensor2.setTimeout(40);
    sensor3.setTimeout(40);
}

void distanceRead() {
    //Armazena os valores lidos nas respectivas variáveis
    dist1 = sensor1.readRangeSingleMillimeters();
    dist2 = sensor2.readRangeSingleMillimeters();
    dist3 = sensor3.readRangeSingleMillimeters();

// Filtro 
//    if(distL > 600) distL = 600;
//    if(distC > 600) distC = 600;
//    if(distR > 600) distR = 600;
}


void printDistances() {
  // Mostra o valor de cada sensor na tela e a decisão escolhida
  Serial.print("1: ");
  Serial.print(dist1);
  Serial.print("\t");
  Serial.print("2: ");
  Serial.print(dist2);
  Serial.print("\t");
  Serial.print("3: ");
  Serial.print(dist3);
  Serial.print("\t");
  Serial.println("\t\t");
}

void setup(){
  Serial.begin(115200);
  sensorsInit();
}

void loop() {
  distanceRead();
  printDistances();
// delay(10);

}
