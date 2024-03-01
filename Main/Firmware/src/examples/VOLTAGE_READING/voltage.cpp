
#include "voltage.h"

voltage::voltage(int PIN)
{
   
    pin = PIN; 
    pinMode(pin,INPUT);
}

void voltage::calibration(int Xa,float Ya, int Xb, float Yb){

    M = (Ya - Yb)/(Xa - Xb) ;
    A = M ;
    B = Ya - Xa*M ; 

   

        Serial.println(" -- END CALIBRATION --");
        Serial.print(" -- M = ");
        Serial.println(M);
        Serial.println(" -- Y = AX + B");
        Serial.print(" -- Y = ");
        Serial.print(A);
        Serial.print("*X + ");
        Serial.println(B);

  


}

float voltage::output(){
    pin_reading = analogRead(pin);

    adc_voltage_reading = pin_reading*M + B - 0.2; 

    return adc_voltage_reading;
    
}

void voltage::debug_print(){
    Serial.print("ADC : ");
    Serial.print(pin_reading);
    Serial.print(" Voltage: ");
    Serial.print(adc_voltage_reading);
}


