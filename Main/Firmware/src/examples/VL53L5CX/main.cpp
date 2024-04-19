// #include <Wire.h>        /*include Wire.h library*/
#include <Arduino.h>
#include "VL53L5CX_sensor.h"
// #include "BluetoothSerial.h"

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// BluetoothSerial SerialBT;

void setup()
{
  Serial.begin(112500);
  // SerialBT.begin("MINI_X_VL"); //Bluetooth device name
  delay(1000);
  VL53L5_init();
 
}

void loop()
{


  //Poll sensor for new data
  VL53L5_debug();
  // int  y = 5;
  // int avg;
  // int sum;
  // Serial.print(imageWidth);

      
  //       for (int x = imageWidth - 1 ; x >= 0 ; x--)
  //       {
  //         Serial.print("\t");
  //         Serial.print("X :");
  //         Serial.print(x);
  //          Serial.print(" ");
  //         Serial.print(VL53L5_data.distance_mm[x + y]);
          
  //       }
  //       Serial.print("\t");
  //       for (int x = imageWidth - 1 ; x >= 0 ; x--)
  //       {
          
  //         avg += VL53L5_data.distance_mm[x + y]*x*1000;
  //         // sum += VL53L5_data.distance_mm[x + y];
          
  //       }

        // float position = avg/sum; 
      //  SerialBT.print(position);
      //  Serial.print(position);
      
  // Serial.println();
  // SerialBT.println();
  delay(10); //Small delay between polling
}
