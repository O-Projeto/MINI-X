
#include "mpu.h"
#include "VL53_sensors.h"

bool _imu_connect; 
bool _connect = false;

#define SCL_SDIST 22
#define SDA_SDIST 21

// XSHUT, uma porta por sensor 
#define SDIST_1 25
#define SDIST_2 23
#define SDIST_3 13

#define N_SENSOR 3 

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2, SDIST_3} ; 

VL53_sensors sensores;









void setup(){

  Serial.begin(112500);

  // check that the IMU initializes correctly
  _imu_connect = imu_setup();

  sensores.sensorsInit();

  if(_imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }
}


void loop(){




  float* imu_ypr = imu_get_ypr();  
  sensores.distanceRead();


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
       Serial.print(sensores.dist[i]);

  }
      Serial.println("\t\t");

  // sensores.testReadDist();
}   

