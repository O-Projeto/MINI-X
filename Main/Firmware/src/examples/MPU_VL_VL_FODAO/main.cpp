#include <Wire.h>        /*include Wire.h library*/
#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h> 
#include "mpu.h"
#include "VL53_sensors.h"

TwoWire Wire2CX((uint8_t)sizeof(uint8_t));

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

static const uint8_t cxSDA = 4;
static const uint8_t cxSCL = 15;

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");
  Wire2CX.setPins(cxSDA,cxSCL);
  Wire2CX.begin();

  // Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

  // myImager.setWireMaxPacketSize(64); //Increase default from 32 bytes to 128 - not supported on all platforms

  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");

  //Time how long it takes to transfer firmware to sensor
  long startTime = millis();
  uint8_t CX_ADDR=0x29;
  bool startup = myImager.begin(CX_ADDR,Wire2CX);
  long stopTime = millis();

  if (startup == false)
  {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1) ;
  }

  Serial.print("Firmware transfer time: ");
  float timeTaken = (stopTime - startTime) / 1000.0;
  Serial.print(timeTaken, 3);
  Serial.println("s");

  myImager.setResolution(8*8); //Enable all 64 pads

  imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution); //Calculate printing width

  myImager.startRanging();

  _imu_connect = imu_setup();

  sensores.sensorsInit();

  if(_imu_connect == 0) {
   
    digitalWrite(2, HIGH); 
  }
}

void loop()
{
  //Poll sensor for new data
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) //Read distance data into array
    {
      //The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          Serial.print("\t");
          Serial.print(measurementData.distance_mm[x + y]);
        }
        // Serial.println();
      }
      // Serial.println();
    }
  }

    float* imu_ypr = imu_get_ypr();  
  sensores.distanceRead();


  //retunr from + pi to -pi 
  Serial.print("Orientation ");
  Serial.print(imu_ypr[0], 5);
  Serial.print(", ");
  Serial.print(imu_ypr[1],5);
  Serial.print(", ");
  Serial.print(imu_ypr[2],5);

   Serial.print(" VL53 :  ");
  for (uint8_t i = 0; i < N_SENSOR; i++){

       Serial.print(" ");
       Serial.print(String(i));
       Serial.print(" ");
       Serial.print(sensores.dist[i]);

  }
      Serial.println("\t\t");
  

  delay(5); //Small delay between polling
}
