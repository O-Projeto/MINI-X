#include <VL53L0X.h> 
#include <vector>

// #define SDIST_1 5 //change the pins 
// #define SDIST_2 23
// #define SDIST_3 13

class VL53_sensors
{
private:
    int number_sensor;
    int* x_shut_pins; // Alterado para ponteiro para os pinos
public:
    std::vector<int> dist;
    int distTest;
    int test = 0;

    std::vector<VL53L0X> sensor;

    
    // Construtor que aceita número de sensores e os pinos
    VL53_sensors(int num, int* pins) : number_sensor(num), x_shut_pins(pins), dist(num), sensor(num) {}
    
    void sensorsInit();
    void distanceRead();
    void printDistances();
    void testRead();
};



void VL53_sensors::sensorsInit() {

    //Iniciando o endereçamento dos sensores
    Wire.begin();

    for (uint8_t i = 0; i < number_sensor; i++){
      pinMode(x_shut_pins[i], OUTPUT);
      digitalWrite(x_shut_pins[i], LOW);


    }

    for (uint8_t i = 0; i < number_sensor; i++){
      pinMode(x_shut_pins[i], INPUT);
      sensor[i].init(true);
      sensor[i].setAddress((uint8_t)0x21 + i); //endereço do sensor 1
      sensor[i].setTimeout(40);
    }
    
}

void VL53_sensors::distanceRead() {
  /*
    for (uint8_t i = 0; i < number_sensor; i++){
       dist[i] = sensor[i].readRangeSingleMillimeters();
      //  Serial.print(" ");
      //  Serial.print(String(i));
      //  Serial.print(" ");
      //  Serial.print(dist[i]);
    }
      //  Serial.println("\t\t");
  */
  for (uint8_t i = 0; i < number_sensor; i++){
       dist[i] = sensor[i].readRangeSingleMillimeters();
  }
}

void VL53_sensors::printDistances() {


  for (uint8_t i = 0; i < number_sensor; i++){

       Serial.print(" ");
       Serial.print(String(i));
       Serial.print(" ");
       Serial.print(dist[i]);

      Serial.println("\t\t");
  }

}


void VL53_sensors::testRead() {
    
        distTest = sensor[test].readRangeSingleMillimeters();
        Serial.print("Serial.printalor do VL ");
        Serial.print(test);
        Serial.print(": ");
        Serial.println(distTest);

}