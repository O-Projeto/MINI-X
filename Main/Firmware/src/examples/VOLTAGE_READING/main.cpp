#include <Arduino.h>

#define PIN 33 

float const A = 1/246.16; 
float const B = 1.7 ;

void setup(){
    Serial.begin(112500);

    pinMode(PIN,INPUT);

}

void loop(){
    int reading = analogRead(PIN);

    float voltage_reading = A*reading + B ;



    Serial.print(reading);
    Serial.print(" ");
    Serial.print(voltage_reading);
    Serial.println("");


}