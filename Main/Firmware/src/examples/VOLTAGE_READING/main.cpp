#include <Arduino.h>
#include "voltage.h"

#define PIN 35

//Vesp =  0,00436997319*Vadc + 0,8616621984

float const A = 0.00436997319; 
float const B = 0.8616621984 ;


voltage battery_voltage(PIN);

void setup(){
    Serial.begin(112500);

    battery_voltage.calibration(2260,11.0,2693,12.63);

}

void loop(){
    battery_voltage.output();
    // int reading = analogRead(PIN);

    // float voltage_reading = A*reading + B ;



    // Serial.print(reading);
    // Serial.print(" ");
    // Serial.print(voltage_reading);
    // Serial.println("");
     Serial.print(battery_voltage.A);
     Serial.print(" ");
     Serial.print(battery_voltage.M);
     Serial.print(" ");
     battery_voltage.debug_print();
     Serial.println("");


}