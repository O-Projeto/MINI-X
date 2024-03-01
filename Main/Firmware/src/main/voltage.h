

#include <Arduino.h>

// This code was writen by Dinossauro Bebado, 2024 
// It reads the adc from the esp32 and converts to voltage,
// the eletronics consists of a voltage divider as follows 

//         VCC ---10k --- Signal --- 2.2K --- GND 
//                         |
//                         |
//                      ESP PIN

// which return this voltage 

//             before |   after 
//             11	  | 1.985
//             11.54  |	2.081
//             12.0   |	2.174
//             12.63  |	2.28

// The original use is to read a lipo 3S battery so the range is from 11 to 12.6 V 

// Now we need to find the  equation of the straight line so Y = AX + B 
// Y beeing the battery voltage and X the ADC reading in the esp32 analog pin 

// --------------------------Calibration-------------------
// The calibration function needs two points to work 
// To figure this points you will need a multimeter 

// First go to the lowest voltage your sistem will get 
//     Now read the voltage with the multimeter- save this value as Ya
//     And using analog read get the adc value the esp32 12 bits resolution will 
//     get you a value from 0 to 4096 and save the value as Xa 
// Repeat the process for the highest voltage 
//     sabe as Yb for the voltage and Xb for the adc 

// Pass this values as parameters for the calibration function and you are set 

//TODO add a mediam filter to stabilize reading 




class voltage
{
private:
   


public:
   float M ; 
   float A ; 
   float B ; 
   float adc_voltage_reading;
   int pin_reading; 
   int pin;
   bool debug = false ; 
    voltage(int PIN);
    void calibration(int Xa,float Ya, int Xb, float Yb);
    void debug_print();
    float output();
    
};
