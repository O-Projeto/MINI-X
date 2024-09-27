#include <Wire.h>        /*include Wire.h library*/
#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h> 

TwoWire Wire2CX((uint8_t)sizeof(uint8_t));

/*
  Read an 8x8 array of distances from the VL53L5CX
  By: Nathan Seidle
  SparkFun Electronics
  Date: October 26, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to setup the I2C bus to minimize the amount
  of time taken to init the sensor.

  At each power on reset, a staggering 86,000 bytes of firmware have to be sent to the sensor.
  At 100kHz, this can take ~9.4s. By increasing the clock speed, we can cut this time down to ~1.4s.

  Two parameters can be tweaked:

    Clock speed: The VL53L5CX has a max bus speed of 1MHz.

    Max transfer size: The majority of Arduino platforms default to 32 bytes. If you are using one
    with a larger buffer (ESP32 is 128 bytes for example), this can help decrease transfer times a bit.

  Measurements:
    Default 100kHz clock and 32 byte transfer: 9.4s
    400kHz, 32 byte transfer: 2.8s
    400kHz, 128 byte transfer: 2.5s
    1MHz, 32 byte transfer: 1.65s
    1MHz, 128 byte transfer: 1.4s

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18642

*/

static const uint8_t cxSDA = 15;
static const uint8_t cxSCL = 4;

float avg ,sum ; 
int menor_dist;
#define STEP 2
struct vl_data {
    int pos;
    int dist;
};

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData VL53L5_data; // Result data class structure, 1356 byes of RAM
vl_data data; 


int imageResolution = 0; //Used to pretty print output
int imageWidth = 0; //Used to pretty print output


void VL53L5_init(){
    Serial.println("SparkFun VL53L5CX Imager Example");
    Wire2CX.setPins(cxSDA,cxSCL);
    Wire2CX.begin();

    Wire2CX.setClock(400000); //Sensor has max I2C freq of 1MHz

    // myImager.setWireMaxPacketSize(32); //Increase default from 32 bytes to 128 - not supported on all platforms

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

    myImager.setResolution(4*4); //Enable all 16 pads
    myImager.setSharpenerPercent(50);
    myImager.setRangingFrequency(15);

    imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
    imageWidth = sqrt(imageResolution); //Calculate printing width

    myImager.startRanging();



}


vl_data VL53L5_get_info(){

  avg = 0; 
  sum = 0; 
  menor_dist = 10000;

    if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&VL53L5_data)) //Read distance data into array
    {
      // The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      // for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      // {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          if(VL53L5_data.distance_mm[x+STEP] < menor_dist){
            menor_dist = VL53L5_data.distance_mm[x+STEP];
          }
          // Serial.print("\t");
          // Serial.print(VL53L5_data.distance_mm[x+STEP]);

          avg += VL53L5_data.distance_mm[x+STEP]*x*1000;
          sum += VL53L5_data.distance_mm[x+STEP];
        }

        data.pos = avg/sum;
        data.dist = menor_dist;
        Serial.print(avg/sum);
        // Serial.println();
        return data;
      // }
      // Serial.println();
    }
  }


}


float VL53L5_get_distance(){

  int closer_distance = 10000; 
 
    if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&VL53L5_data)) //Read distance data into array
    {
      // The ST library returns the data transposed from zone mapping shown in datasheet
      //Pretty-print data with increasing y, decreasing x to reflect reality
      // for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      // {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          // Serial.print("\t");
          // Serial.print(VL53L5_data.distance_mm[x]);
          if(VL53L5_data.distance_mm[x]<closer_distance){
              closer_distance = VL53L5_data.distance_mm[x];
          }
        
        }
        
        return closer_distance;
     
    }
  }

}

// int* get_VL53_mm(){

//     return measurementData.distance_mm ;


// }