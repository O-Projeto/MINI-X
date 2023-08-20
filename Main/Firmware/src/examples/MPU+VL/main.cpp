
#include "mpu.h"
#include <VL53L0X.h> 

bool _imu_connect; 
bool _connect = false;

#define SCL_SDIST 22
#define SDA_SDIST 21

// XSHUT, uma porta por sensor 
#define SDIST_1 25
#define SDIST_2 23
#define SDIST_3 13

#define N_SENSOR 3 


//Cria os objetos para cada sensor
VL53L0X sensor[3];  //Sensor da esquerda


int dist[N_SENSOR] ;

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2, SDIST_3} ; 




void sensorsInit() {

    //Iniciando o endereçamento dos sensores
    Wire.begin();

    for (uint8_t i = 0; i < N_SENSOR; i++){
      pinMode(X_SHUT[i], OUTPUT);
      digitalWrite(X_SHUT[i], LOW);


    }

    for (uint8_t i = 0; i < N_SENSOR; i++){
      pinMode(X_SHUT[i], INPUT);
      sensor[i].init(true);
      sensor[i].setAddress((uint8_t)0x21 + i); //endereço do sensor 1
      sensor[i].setTimeout(40);
    }
    
}


void distanceRead() {

    for (uint8_t i = 0; i < N_SENSOR; i++){
       dist[i] = sensor[i].readRangeSingleMillimeters();
    }
   
}


void printDistances() {


  for (uint8_t i = 0; i < N_SENSOR; i++){

       Serial.print(" ");
       Serial.print(String(i));
       Serial.print(" ");
       Serial.print(dist[i]);

      Serial.println("\t\t");
  }

}



void setup(){

  Serial.begin(112500);

  // check that the IMU initializes correctly
  _imu_connect = imu_setup();

  sensorsInit();

  if(_imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }
}


void loop(){




  float* imu_ypr = imu_get_ypr();  
  distanceRead();


  //retunr from + pi to -pi 
  Serial.print("Orientation ");
  Serial.print(imu_ypr[0], 5);
  Serial.print(", ");
  Serial.print(imu_ypr[1],5);
  Serial.print(", ");
  Serial.print(imu_ypr[2],5);


  for (uint8_t i = 0; i < N_SENSOR; i++){

       Serial.print(" ");
       Serial.print(String(i));
       Serial.print(" ");
       Serial.print(dist[i]);

  }
      Serial.println("\t\t");
  
}   

