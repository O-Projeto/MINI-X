
// #include "mpu.h"
#include <Arduino.h>
// #include <VL53L0X.h> 
#include "VL53_sensors.h"


#define SCL_SDIST 22
#define SDA_SDIST 21

// XSHUT, uma porta por sensor 
#define SDIST_1 25
#define SDIST_2 23
#define SDIST_3 13

#define N_SENSOR 3
int x_shut_pins[N_SENSOR] = {SDIST_1, SDIST_2, SDIST_3};
int x_shut_pins_2[1] = {-1};

TwoWire Wire2CX((uint8_t)sizeof(uint8_t));
static const uint8_t cxSDA = 15;
static const uint8_t cxSCL = 4;

VL53_sensors sensores(N_SENSOR, x_shut_pins);
VL53_sensors sensor(1, x_shut_pins_2);

void setup(){

  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  
  Wire2CX.setPins(cxSDA,cxSCL);
  Wire2CX.begin();
  Wire2CX.setClock(400000); //Sensor has max I2C freq of 1MHz

  sensores.sensorsInit(Wire);
  sensor.sensorsInit(Wire2CX);

}


void loop(){ 
  // sensores.testRead();
  // sensor.testRead();

  sensores.distanceRead();
  sensor.distanceRead();


  for (uint8_t i = 0; i < N_SENSOR; i++){

       Serial.print(" ");
       Serial.print(String(i));
       Serial.print(" ");
       Serial.print(sensores.dist[i]);

  }
      Serial.println("\t\t");
  Serial.print("VL I2C2: ");
  Serial.println(sensor.dist[0]);

}   

