// #include <Wire.h>        /*include Wire.h library*/
#include <Arduino.h>
#include "VL53L5CX_sensor.h"

void setup()
{
  Serial.begin(112500);
  delay(1000);
  VL53L5_init();
 
}

void loop()
{
  //Poll sensor for new data
  VL53L5_debug();
  for (int y = 0 ; y <= imageWidth * (imageWidth - 1) ; y += imageWidth)
      {
        for (int x = imageWidth - 1 ; x >= 0 ; x--)
        {
          Serial.print("\t");
          Serial.print(VL53L5_data.distance_mm[x + y]);
        }
        Serial.println();
      }
      Serial.println();
  delay(100); //Small delay between polling
}
