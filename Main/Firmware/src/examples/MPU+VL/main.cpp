#include <Arduino.h>
#include "mpu.h"
#include <VL53L0X.h>


#define SCL_SDIST 22
#define SDA_SDIST 21

// XSHUT, uma porta por sensor 
#define SDIST_1 23
#define SDIST_2 19

VL53L0X sensor1;  //Sensor da esquerda
VL53L0X sensor2;  //Sensor da frente

bool _imu_connect; 
bool _connect = false;

void sensorsInit() {

    //Iniciando o endereçamento dos sensores
   

    pinMode(SDIST_1, OUTPUT);
    pinMode(SDIST_2, OUTPUT);


    digitalWrite(SDIST_1, LOW);
    digitalWrite(SDIST_2, LOW);

    pinMode(SDIST_1, INPUT);
    sensor1.init(true);
    sensor1.setAddress((uint8_t)0x21); //endereço do sensor 1

    pinMode(SDIST_2, INPUT);
    sensor2.init(true);
    sensor2.setAddress((uint8_t)0x22); //endereço do sensor 2


    sensor1.setTimeout(20);
    sensor2.setTimeout(20);

}



// ----------- ultrasonic sensor - distance

void setup(){

  Serial.begin(112500);


  // check that the IMU initializes correctly
  _imu_connect = imu_setup();

  sensorsInit();

  if(_imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }

  sensor1.setTimeout(500);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor 1!");
    while (1) {}
  }

  sensor2.setTimeout(500);

  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor 2 !");
    while (1) {}
  }
}


void loop(){




  float* imu_ypr = imu_get_ypr();  
  //retunr from + pi to -pi 
  Serial.print("Orientation ");
  Serial.print(imu_ypr[0], 5);
  Serial.print(", ");
  Serial.print(imu_ypr[1],5);
  Serial.print(", ");
  Serial.print(imu_ypr[2],5);
  Serial.print(", ");
  Serial.print(sensor1.readRangeSingleMillimeters());
  Serial.print(", ");
  Serial.print(sensor2.readRangeSingleMillimeters());
  Serial.println("");
  
}   