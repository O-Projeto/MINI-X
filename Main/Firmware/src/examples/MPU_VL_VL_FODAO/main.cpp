
#include "mpu.h"
#include "VL53_sensors.h"
#include <SparkFun_VL53L5CX_Library.h> 

bool _imu_connect; 
bool _connect = false;

#define SCL_SDIST 22
#define SDA_SDIST 21
#define SCL_CX 17
#define SDA_CX 16


// XSHUT, uma porta por sensor 
#define SDIST_1 25
#define SDIST_2 23
#define SDIST_3 13

#define N_SENSOR 3 

int X_SHUT[N_SENSOR] = { SDIST_1, SDIST_2, SDIST_3} ; 

VL53_sensors sensores;

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output


void i2cScanner()
{
  while (1)
  {
    Serial.println(F("Scanning for I2C devices"));
    for (byte address = 1; address < 127; address++ )
    {
      Wire.beginTransmission(address);
      if (Wire.endTransmission() == 0)
      {
        Serial.print("Device found at address 0x");
        if (address < 0x10)
          Serial.print("0");
        Serial.println(address, HEX);
      }
    }
    Serial.println("Done");
    delay(250);
  }
}




void setup(){

  Serial.begin(112500);


  // check that the IMU initializes correctly
  _imu_connect = imu_setup();

  sensores.sensorsInit();

  if(_imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }

  

  // myImager.begin();

 
  //   int deviceAddress = 0x44; //Valid: 0x08 <= address <= 0x77
  //   myImager.setAddress(deviceAddress);

  //   delay(500);

  //   int newAddress = myImager.getAddress();

  //   myImager.setResolution(8 * 8); //Enable all 64 pads

  //   imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
  //   imageWidth = sqrt(imageResolution); //Calculate printing width

  //   myImager.startRanging();
  
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

  //   if (myImager.isDataReady() == true)
  // {
  //   if (myImager.getRangingData(&measurementData)) //Read distance data into array
  //   {
  //     //The ST library returns the data transposed from zone mapping shown in datasheet
  //     //Pretty-print data with increasing y, decreasing x to reflect reality
  //     for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
  //     {
  //       for (int x = imageWidth - 1 ; x >= 0 ; x--)
  //       {
  //         Serial.print("\t");
  //         Serial.print(measurementData.distance_mm[x + y]);
  //       }
  //       Serial.println();
  //     }
  //     Serial.println();
  //   }
  // }


      Serial.println("\t\t");
  
}   

